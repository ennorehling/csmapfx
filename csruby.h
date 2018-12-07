/*
  Copyright (C) 2007 Thomas J. Gritzan

  csruby.h - embedded ruby in c++ library

  Created: 30.05.07
*/

#ifndef CSRUBY_H_
#define CSRUBY_H_

#include <stdexcept>
#include <ostream>
#include <string>

//#include <function>
//#include <ruby.h>

namespace Ruby
{
	// start and stop ruby subsystems
	void initialize();
	void finalize();

	// ruby value wrapper
	class Value
	{
	public:
		class ValueImpl;

		Value();	// nil
		Value(const Value& val);
		~Value();
		explicit Value(const std::string& str);	// ruby string
		explicit Value(int num);
		explicit Value(ValueImpl* pimpl);

#ifdef RUBY_H
#define RUBY_H_IN_CSRUBY
		explicit Value(VALUE val);
		operator VALUE() const;
#endif

		Value& operator=(const Value& val);

		ValueImpl* get() const;

		std::string to_s() const;

		void swap(Value& val);
	private:
		ValueImpl* pimpl;
	};

	// ruby function wrapper
	// catches ruby exceptions and translates them into C++ exceptions
	void Require(const std::string& filename);
	
	Value Evaluate(const std::string& str);

	Value New(const std::string& klass);
	Value New(const std::string& klass, Value arg1);
	Value New(const std::string& klass, Value arg1, Value arg2);

	Value Funcall(Value recv, const std::string& fun);
	Value Funcall(Value recv, const std::string& fun, Value arg1);
	Value Funcall(Value recv, const std::string& fun, Value arg1, Value arg2);

	Value DefineModule(const std::string& name);
	Value DefineClass(const std::string& name);
	Value DefineClassUnder(Value module, const std::string& name);

	template <typename FUN>
	void DefineMethod(Value klass, const std::string& name, FUN fun, int argc)
	{
		DefineMethod(klass, name, (void*)fun, argc);
	}

	template <>
	void DefineMethod(Value klass, const std::string& name, void* fun, int argc);

	// translate a ruby exception into a C++ exception.
	/* usage:
		void Test() {
			int error = 0;
			rb_protect(WrapTest, reinterpret_cast<VALUE>(this), &error);
			if(error)
				throw Ruby::Error("error loading test.rb");
		}
	*/
	class Error : public std::exception
	{
	public:
		Error(const std::string& name);
		virtual ~Error() throw();
		virtual const char* what() const throw();
		virtual std::string str() const;
		virtual void explain(std::ostream& o) const throw();

		friend std::ostream& operator<<(std::ostream& out, const Error& err)
		{
			err.explain(out);
			return out;
		}

	private:
		std::string name;
		std::string where;
		std::string message;
		std::string klass;
		std::string backtrace;
	};

	// translate from C++ to ruby exceptions
	/*
		adapted from
		http://groups.google.com/groups?hl=en&lr=&ie=UTF-8&selm=20020408192019.E15413%40atdesk.com&rnum=10

		usage:
		setException();
		try
		{
			// ...
		}
		catch (const std::exception& err)
		{
			Ruby::setException(err);
		}
		Ruby::raise();

	*/
	void setException();
	void setException(const std::string& err);
	void setException(const std::exception& err);
	void raise();

}	// ::Ruby

#endif //CSRUBY_H_
