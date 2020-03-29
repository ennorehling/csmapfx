/*
  Copyright (C) 2007 Thomas J. Gritzan

  csruby.cpp - embedded ruby in c++ library

  Created: 30.05.07
*/

#include <stdexcept>
#include <string>
#include <sstream>

#define RUBY_EXPORT
#include <ruby.h>
#include "csruby.h"

extern "C" VALUE ruby_errinfo;

namespace Ruby
{
	/***********************************
	* memory management
	***********************************/
	class Objects
	{
	public:
		Objects();
		~Objects();

		void Register(VALUE object);
		void Unregister(VALUE object);
	private:
		VALUE objects;
	};

	static Objects* objects = 0;

	/***********************************
	* starts and stops ruby subsystems
	***********************************/
	static bool ruby_running = false;

	void initialize()
	{
		if (ruby_running)
			return;

#ifdef _WIN32
    	//NtInitialize(&argc, &argv);
#endif

		// initialize ruby itself
		ruby_init();
		putenv("RUBYLIB=.");	// to make require 'script' work
		ruby_init_loadpath();
		ruby_script("embed");

		// inject int/float division hack
		const char* real_division_hack =
			"class Fixnum\n"
			"    alias :div :/\n"
			"    def /(other)\n"
			"        if self % other == 0\n"
			"            self.div(other)\n"
			"        else\n"
			"            1.0*self/other\n"
			"        end\n"
			"    end\n"
			"end";

		int error;
		rb_eval_string_protect(real_division_hack, &error);

		// setup our own environment
		objects = new Objects;

		ruby_running = true;
	}

	void finalize()
	{
		if (!ruby_running)
			return;

		// teardown (reverse initialization order)
		delete objects;
		ruby_finalize();

		ruby_running = false;
	}

	/*************************************
	* ruby exceptions into c++ exceptions
	*************************************/
	Error::Error(const std::string& name) : name(name)
	{
#ifdef TODO_RUBY
            std::ostringstream where;
                where << ruby_sourcefile << ":" << ruby_sourceline;
                ID id = rb_frame_last_func();
		if(id) {
			where << ":in '" << rb_id2name(id) << "'";
		}
		this->where = where.str();
#endif

		VALUE exception_instance = rb_gv_get("$!");

		// class
		VALUE klass = rb_class_path(CLASS_OF(exception_instance));
		this->klass = RSTRING_PTR(klass);

		// message
		VALUE message = rb_obj_as_string(exception_instance);
                this->message = RSTRING_PTR(message);

#ifdef TODO_RUBY
                // backtrace
		if(!NIL_P(ruby_errinfo)) {
			std::ostringstream trace;
			VALUE ary = rb_funcall(ruby_errinfo, rb_intern("backtrace"), 0);
			for (int i = 0; i < RARRAY_LEN(ary); i++) {
                            trace << "\tfrom " << RSTRING_PTR(RARRAY_PTR(ary)[i]) << "\n";
			}
			this->backtrace = trace.str();
		}
#endif
	}

	Error::~Error() throw()
	{
	}

	const char* Error::what() const throw()
	{
		return name.c_str();
	}

	std::string Error::str() const
	{
		return "#<" + klass + ": " + where + ": " + message + ">";
	}

	void Error::explain(std::ostream& out) const throw()
	{
		out <<
			"name=" << name << "\n"
			"where=" << where << "\n"
			"class=" << klass << "\n"
			"message=" << message << "\n"
			"backtrace=" << backtrace << std::endl;
	}

	/*************************************
	* memory management
	*************************************/
	Objects::Objects()
	{
		objects = rb_ary_new();
		rb_gc_register_address(&objects);
	}

	Objects::~Objects()
	{
		// dispose array and flush all elements
		rb_gc_unregister_address(&objects);
		/*
		mass destruction.
		GC can no longer mark the elements in
		the Array and therefore they will all get swept.
		*/
	}

	void Objects::Register(VALUE object)
	{
		rb_ary_push(objects, object);
	}

	void Objects::Unregister(VALUE object)
	{
		rb_ary_delete(objects, object);
	}

	/*************************************
	* value wrapper
	*************************************/
	inline Value::ValueImpl* VALUE2IMPL(VALUE val) { return reinterpret_cast<Value::ValueImpl*>(val); }
	inline VALUE IMPL2VALUE(Value::ValueImpl* pimpl) { return reinterpret_cast<VALUE>(pimpl); }

	Value::Value() : pimpl(VALUE2IMPL(Qnil)) {}
	Value::Value(const Value& val) : pimpl(val.pimpl) {}
	Value::~Value() {}
	/*explicit*/ Value::Value(const std::string& str) : pimpl(VALUE2IMPL(rb_str_new2(str.c_str()))) {}
	/*explicit*/ Value::Value(int num) : pimpl(VALUE2IMPL(INT2NUM(num))) {}
	/*explicit*/ Value::Value(Value::ValueImpl* val) : pimpl(val) {}

#ifdef RUBY_H_IN_CSRUBY
	/*explicit*/ Value::Value(VALUE val) : pimpl(VALUE2IMPL(val)) {}
	Value::operator VALUE() const { return IMPL2VALUE(pimpl); }
#endif

	Value& Value::operator=(const Value& val)
	{
		Value temp(val);
		this->swap(temp);
		return *this;
	}

	Value::ValueImpl* Value::get() const
	{
		return pimpl;
	}

	std::string Value::to_s() const
	{
		VALUE val = IMPL2VALUE(pimpl);

		if (TYPE(val) == T_STRING)
			return std::string(RSTRING_PTR(val), RSTRING_LEN(val));
		else
		{
			int error = 0;
			VALUE str = rb_protect(rb_String, val, &error);
			if (error)
				throw Ruby::Error("converting to string failed");
			if (TYPE(str) != T_STRING)
				throw std::runtime_error("to_s returns no string");
			return std::string(RSTRING_PTR(str), RSTRING_LEN(str));
		}
	}

	void Value::swap(Value& val)
	{
		ValueImpl* temp = val.pimpl;
		val.pimpl = pimpl;
		pimpl = temp;
	}

	/*************************************
	* wrap rb_class_new_instance
	*************************************/
	struct NewArguments
	{
		const char *klass;
		int n;
		VALUE *argv;
		NewArguments(const char *klass, int n, VALUE *argv) :
			klass(klass), n(n), argv(argv) {}
	};

	VALUE NewWrap(VALUE arg)
	{
		NewArguments &a = *reinterpret_cast<NewArguments*>(arg);
		VALUE klass = rb_const_get(rb_cObject, rb_intern(a.klass));
		/*
		Apparently there is a difference between
			a) rb_class_new_instance(a.n, a.argv, klass);
			b) rb_funcall2(klass, rb_intern("new"), a.n, a.argv);
		Using (a) then we cannot inheirit from a SWIG class.
		Using (b) then there is no problems.

		We don't want problems, so we choose (b)!
		todo: why does (a) not work?

		Using (a) we get this error message:
			EXCEPTION (RUBY):
			name=cannot invoke ruby-function
			where=./test.rb:21
			class=TypeError
			message=wrong argument type RubyView (expected Data)
			backtrace=      from ./test.rb:21:in `repaint'
					from ./test.rb:21:in `insert'
		*/
		//VALUE self = rb_class_new_instance(a.n, a.argv, klass);
		VALUE self = rb_funcall2(klass, rb_intern("new"), a.n, a.argv);
		return self;
	}

	Value New(const std::string& klass, int n, VALUE* values)
	{
		NewArguments arg(klass.c_str(), n, values);
		int error = 0;
		VALUE self = rb_protect(NewWrap, reinterpret_cast<VALUE>(&arg), &error);
		if (error)
		{
			std::ostringstream o;
			o << "error creating " << klass;
			throw Error(o.str());
		}
		return Value(VALUE2IMPL(self));
	}

	Value New(const std::string& klass)
	{
		return New(klass, 0, 0);
	}

	Value New(const std::string& klass, Value arg1)
	{
		VALUE args[] = { IMPL2VALUE(arg1.get()) };
		return New(klass, sizeof(args)/sizeof(*args), args);
	}

	Value New(const std::string& klass, Value arg1, Value arg2)
	{
		VALUE args[] = { IMPL2VALUE(arg1.get()), IMPL2VALUE(arg2.get()) };
		return New(klass, sizeof(args)/sizeof(*args), args);
	}

	/*************************************
	* wrap rb_funcall
	*************************************/
	struct Arguments
	{
		VALUE recv_;
		ID id;
		int n;
		VALUE *argv;
		Arguments(VALUE recv, ID id, int n, VALUE *argv) :
			recv_(recv), id(id), n(n), argv(argv)
		{
		}
	};

	VALUE FuncallWrap(VALUE arg)
	{
		Arguments &a = *reinterpret_cast<Arguments*>(arg);
		return rb_funcall2(a.recv_, a.id, a.n, a.argv);
	}

	/*
	call a ruby function in a safe way.
	translate ruby errors into C++ exceptions.

	VALUE Unsafe() {
		return rb_funcall(
			self,
			rb_intern("test"),
			1,
			INT2NUM(42)
		);
	}

	VALUE Safe() {
		return Ruby::Funcall(
			self,
			rb_intern("test"),
			1,
			INT2NUM(42)
		);
	}
	*/
	Value Funcall(const Value& recv, ID id, int n, VALUE* values)
	{
		Arguments arg(IMPL2VALUE(recv.get()), id, n, values);
		int error = 0;
		VALUE result = rb_protect(FuncallWrap, reinterpret_cast<VALUE>(&arg), &error);
		if (error)
			throw Error("cannot invoke ruby-function");
		return Value(VALUE2IMPL(result));
	}

	Value Funcall(Value recv, const std::string& fun)
	{
		return Funcall(recv, rb_intern(fun.c_str()), 0, 0);
	}

	Value Funcall(Value recv, const std::string& fun, Value arg1)
	{
		VALUE args[] = { IMPL2VALUE(arg1.get()) };
		return Funcall(recv, rb_intern(fun.c_str()), sizeof(args)/sizeof(*args), args);
	}

	Value Funcall(Value recv, const std::string& fun, Value arg1, Value arg2)
	{
		VALUE args[] = { IMPL2VALUE(arg1.get()), IMPL2VALUE(arg2.get()) };
		return Funcall(recv, rb_intern(fun.c_str()), sizeof(args)/sizeof(*args), args);
	}

	/*************************************
	* wrap rb_require
	*************************************/
	VALUE RequireWrap(VALUE arg)
	{
		const char *filename = reinterpret_cast<const char*>(arg);
		rb_require(filename);
		return Qnil;
	}

	// require a ruby-file in a safe way
	void Require(const std::string& filename)
	{
		int error = 0;
		rb_protect(RequireWrap, reinterpret_cast<VALUE>(filename.c_str()), &error);
		if(error) {
			std::ostringstream o;
			o << "error loading " << filename << ".rb";
			throw Error(o.str());
		}
	}

	/*************************************
	* wrap rb_eval_string_protect
	*************************************/

	Value Evaluate(const std::string& str)
	{
		int error = 0;
		Value value = Value(rb_eval_string_protect(str.c_str(), &error));
		if(error)
			throw Error("error evaluating string");

		return value;
	}

	/*************************************
	* wrap rb_define_*
	*************************************/

	Value DefineModule(const std::string& name)
	{
		return Value(rb_define_module(name.c_str()));
	}

	Value DefineClass(const std::string& name)
	{
		return Value(rb_define_class(name.c_str(), rb_cObject));
	}

	Value DefineClassUnder(Value module, const std::string& name)
	{
		return Value(rb_define_class_under(module, name.c_str(), rb_cObject));
	}

	template <>
	void DefineMethod(Value klass, const std::string& name, void* fun, int argc)
	{
		rb_define_method(klass, name.c_str(), RUBY_METHOD_FUNC(fun), argc);
	}

	/*************************************
	* c++ exceptions into ruby exceptions
	*************************************/
	/*
		adapted from
		http://groups.google.com/groups?hl=en&lr=&ie=UTF-8&selm=20020408192019.E15413%40atdesk.com&rnum=10

		RUBY_CATCH
		Cant raise the exception from catch block, because
		the C++ exception wont get properly destroyed.
		Thus we must delay it til after the catch block!
	*/

	void setException()
	{
#ifdef TODO_RUBY
            ruby_errinfo = Qnil;
#endif
	}

	void setException(const std::string& err)
	{
#ifdef TODO_RUBY
            ruby_errinfo = rb_exc_new2(rb_eRuntimeError, err.c_str());
#endif
	}

	void setException(const std::exception& err)
	{
		std::ostringstream o;
		o << "c++error: " << err.what();
#ifdef TODO_RUBY
                ruby_errinfo = rb_exc_new2(rb_eRuntimeError, o.str().c_str());
#endif
	}

	void raise()
	{
#ifdef TODO_RUBY
            if (!NIL_P(ruby_errinfo))
			rb_exc_raise(ruby_errinfo);
#endif
	}

}	// ::Ruby
