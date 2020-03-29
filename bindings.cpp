#include "main.h"
#include "fxhelper.h"

#include <fx.h>
#include <list>
#include <string>
#include "datafile.h"
#include "csmap.h"

#define RUBY_EXPORT
#undef strcasecmp
#include <ruby.h>
#undef bind
#include "csruby.h"
#include "bindings.h"

namespace bindings
{
	using Ruby::Value;

	/***********************************
	* forward declarations
	***********************************/

	nil_t nil;

	/***********************************
	* little helpers
	***********************************/
	Value make_str(const FXString& str)
	{
        return Value(utf2iso(str).text());
	}

	/***********************************
	* ruby Block class
	***********************************/
	class Block
	{
	public:
		Block(filelist_t& files, block_itor block, block_itor end = block_itor()) : m_files(&files), m_block(block), m_end(end)
		{
			if (end == block_itor())
				if (!m_files->empty())
					m_end = m_files->begin()->end();

			m_signal = CSMap::getInstance()->register_map_change(boost::bind(&Block::mapChange, this));
		}

		~Block()
		{
			m_signal.disconnect();
		}

		void mapChange()
		{
			m_block = m_end;
		}

		static void mark(Block* obj) { }
		static void free(Block* obj) { delete obj; }

		static Block* get_block(VALUE self)
		{
			Block* block;
			Data_Get_Struct(self, Block, block);

			if (!block || block->m_block == block->m_end)
				rb_raise(rb_eRuntimeError, "Block is invalid");

			return block;
		}

		static VALUE makenew(VALUE klass, VALUE arg);	// defined after Tags class

		static VALUE to_s(VALUE self)
		{
			Block* block_p = get_block(self);
			datablock::itor block = block_p->m_block;

			FXString str = block->string();
			int flags = (block->flags() & (datablock::FLAG_BLOCKID_BIT0|datablock::FLAG_BLOCKID_BIT1)) / datablock::FLAG_BLOCKID_BIT0;
			if (flags >= 2)
				str += " " + FXStringVal(block->x()) + " " + FXStringVal(block->y());
			if (flags == 1 || flags == 3)
				str += " " + FXStringVal(block->info());

            return make_str(str);
		}

		static VALUE tags(VALUE self)
		{
			Block* block_p = get_block(self);

			return bind_tags(*block_p->m_files, block_p->m_block);
		}

		static VALUE blocks(VALUE self)
		{
			Block* block_p = get_block(self);

			return bind_blocklist(*block_p->m_files, block_p->m_block);
		}

		static VALUE type(VALUE self)
		{
			Block* block_p = get_block(self);

			return make_str(block_p->m_block->string());
		}

		static VALUE id(VALUE self)
		{
			Block* block_p = get_block(self);
			datablock::itor block = block_p->m_block;
			
			int flags = (block->flags() & (datablock::FLAG_BLOCKID_BIT0|datablock::FLAG_BLOCKID_BIT1)) / datablock::FLAG_BLOCKID_BIT0;
			if (flags == 0)
				return Qnil;
			else if (flags == 1)
				return Value(block->info());
			
			VALUE array = rb_ary_new2(flags);
			rb_ary_push(array, Value(block->x()));
			rb_ary_push(array, Value(block->y()));
			if (flags > 2)
				rb_ary_push(array, Value(block->info()));

			return array;
		}

		static VALUE id36(VALUE self)
		{
			Block* block_p = get_block(self);
			datablock::itor block = block_p->m_block;
			
			int flags = (block->flags() & (datablock::FLAG_BLOCKID_BIT0|datablock::FLAG_BLOCKID_BIT1)) / datablock::FLAG_BLOCKID_BIT0;
			if (flags == 1)
				return make_str(block->id());

			return id(self);
		}

		static VALUE terrain(VALUE self)
		{
			Block* block_p = get_block(self);
			datablock::itor block = block_p->m_block;
			
			if (block->type() == datablock::TYPE_REGION)
				return make_str(block->terrainString());

			return Qnil;
		}

	public:
		static Value klass;		// instance of the ruby class object

	protected:
		filelist_t* m_files;
		block_itor m_block, m_end;
		connection_t m_signal;
	};

	/*static*/ Value Block::klass;

	/***********************************
	* ruby BlockList class
	***********************************/
	class BlockList : public Block
	{
	public:
		BlockList(filelist_t& files, block_itor block, const std::string& type, block_itor end = block_itor())
			: Block(files, block, end), m_size(-1)
		{
			 m_type = iso2utf(type.c_str());
		}

		static void mark(BlockList* obj) { }
		static void free(BlockList* obj) { delete obj; }

		static BlockList* get_block(VALUE self)
		{
			return static_cast<BlockList*>(Block::get_block(self));
		}

		static VALUE makenew(VALUE klass, VALUE arg)
		{
			if (rb_obj_is_kind_of(arg, Block::klass) == Qtrue)
				return rb_funcall(arg, rb_intern("blocks"), 0);

			rb_raise(rb_eTypeError, "BlockList.new must ne called with a Block parameter.");
			return Qnil;
		}

		static VALUE each(VALUE self)
		{
			BlockList* list_p = get_block(self);
			datablock::itor block = list_p->m_block;

			int depth = block->depth();
			datablock::itor itor = block;
			datablock::itor end = list_p->m_end;
			for (itor++; itor != end && depth < itor->depth(); itor++)
			{
                if (depth+1 == itor->depth())
					if (list_p->m_type.empty() || itor->string() == list_p->m_type)
						rb_yield(bind_block(*list_p->m_files, itor));
			}

			return self;
		}

		static VALUE size(VALUE self)
		{
			BlockList* list_p = get_block(self);
			datablock::itor block = list_p->m_block;

			if (list_p->m_size == -1)
			{
				int size = 0;
				int depth = block->depth();
				datablock::itor itor = block;
				datablock::itor end = list_p->m_end;
				for (itor++; itor != end && depth < itor->depth(); itor++)
				{
                    if (depth+1 == itor->depth())
						if (list_p->m_type.empty() || itor->string() == list_p->m_type)
							size++;
				}

				list_p->m_size = size;
			}

			return INT2NUM(list_p->m_size);
		}

		static VALUE index_op(int argc, VALUE *argv, VALUE self)
		{
			BlockList* list_p = get_block(self);
			datablock::itor block = list_p->m_block;

			if (argc < 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1 or more)", argc);

			VALUE arg = argv[0];
			if (TYPE(arg) == T_SYMBOL || TYPE(arg) == T_STRING)
			{
				const char* key;
				if (TYPE(arg) == T_SYMBOL)
					key = rb_id2name(rb_to_id(arg));
				else
                                    key = StringValuePtr(arg);

				FXString type = iso2utf(key);

				int depth = block->depth();
				datablock::itor itor = block;
				datablock::itor end = list_p->m_end;
				for (itor++; itor != end && depth < itor->depth(); itor++)
					if (depth+1 == itor->depth() && itor->string() == type)
						if (list_p->m_type.empty() || itor->string() == list_p->m_type)
						{
							// if block has no flags and only one argument given, then return block
							int flags = (itor->flags() & (datablock::FLAG_BLOCKID_BIT0|datablock::FLAG_BLOCKID_BIT1)) / datablock::FLAG_BLOCKID_BIT0;
							if (!flags && argc == 1)
								return bind_block(*list_p->m_files, itor);

							// if block has flags but only one argument given, then return BlockList bound to type()==key
							if (flags && argc == 1)
								return bind_blocklist(*list_p->m_files, block, key);	// BlockList.begin = self.begin!

							// else, search for a block with type()==key and ids()==args
							int i;
							int indices[3] = { itor->x(), itor->y(), itor->info() };
							if (flags == 1)
								indices[0] = itor->info();
							for (i = 0; i < argc-1 && i < flags; i++)
							{
								int number = 0;

								VALUE arg = argv[i+1];
								if (TYPE(arg) == T_FIXNUM)
									number = FIX2INT(arg);
								else if (TYPE(arg) == T_STRING)
                                                                    number = strtol(StringValuePtr(arg), NULL, 36);

								if (number != indices[i])
									break;
							}

							// if loop finished, all are identical
							if (i == argc-1 && i == flags)
								return bind_block(*list_p->m_files, itor);
						}

				return Qnil;
			}

			// else
			rb_raise(rb_eTypeError, "wrong type of argument. Only strings and symbols are allowed.");
			return Qnil;
		}

	public:
		static Value klass;		// instance of the ruby class object

	protected:
		int m_size;				// cached result of #size method
		FXString m_type;
	};

	/*static*/ Value BlockList::klass;

	/***********************************
	* ruby Tags class
	***********************************/
	class Tags : public Block
	{
	public:
		Tags(filelist_t& files, block_itor block, block_itor end = block_itor()) : Block(files, block, end)
		{
		}

		static void mark(Tags* obj) { }
		static void free(Tags* obj) { delete obj; }

		static Tags* get_block(VALUE self)
		{
			return static_cast<Tags*>(Block::get_block(self));
		}

		static VALUE makenew(VALUE klass, VALUE arg)
		{
			if (rb_obj_is_kind_of(arg, Block::klass) == Qtrue)
				return rb_funcall(arg, rb_intern("tags"), 0);

			rb_raise(rb_eTypeError, "Tags.new must ne called with a Block parameter.");
			return Qnil;
		}

		static VALUE each(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			if (block->type() == datablock::TYPE_REGION)
				rb_yield(rb_ary_new3(2, rb_str_new2("Terrain"), (VALUE)make_str(block->terrainString())));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
			{
				VALUE value;
				if (itor->isInt())
					value = INT2NUM(itor->getInt());
				else
					value = make_str(itor->value());

				rb_yield(rb_ary_new3(2, (VALUE)make_str(itor->key()), value));
			}

			return self;
		}

		static VALUE each_pair(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			if (block->type() == datablock::TYPE_REGION)
				rb_yield_values(2, rb_str_new2("Terrain"), (VALUE)make_str(block->terrainString()));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
			{
				VALUE value;
				if (itor->isInt())
					value = INT2NUM(itor->getInt());
				else
					value = make_str(itor->value());

				rb_yield_values(2, (VALUE)make_str(itor->key()), value);
			}

			return self;
		}

		static VALUE each_key(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			if (block->type() == datablock::TYPE_REGION)
				rb_yield(rb_str_new2("Terrain"));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
				rb_yield(make_str(itor->key()));

			return self;
		}

		static VALUE each_value(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			if (block->type() == datablock::TYPE_REGION)
				rb_yield(make_str(block->terrainString()));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
			{
				VALUE value;
				if (itor->isInt())
					value = INT2NUM(itor->getInt());
				else
					value = make_str(itor->value());

				rb_yield(value);
			}

			return self;
		}

		static VALUE keys(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			size_t size = block->data().size();
			if (block->type() == datablock::TYPE_REGION)
				size++;

			VALUE array = rb_ary_new2(size);

			if (block->type() == datablock::TYPE_REGION)
				rb_ary_push(array, rb_str_new2("Terrain"));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
				rb_ary_push(array, make_str(itor->key()));

			return array;
		}

		static VALUE values(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			size_t size = block->data().size();
			if (block->type() == datablock::TYPE_REGION)
				size++;

			VALUE array = rb_ary_new2(size);

			if (block->type() == datablock::TYPE_REGION)
				rb_ary_push(array, make_str(block->terrainString()));

            datakey::list_type &keys = block->data();
			for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
			{
				VALUE value;
				if (itor->isInt())
					value = INT2NUM(itor->getInt());
				else
					value = make_str(itor->value());

				rb_ary_push(array, value);
			}

			return array;
		}

		static VALUE size(VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			size_t size = block->data().size();
			if (block->type() == datablock::TYPE_REGION)
				size++;

			return INT2NUM(size);
		}

		static VALUE index_op(int argc, VALUE *argv, VALUE self)
		{
			Tags* tags_p = get_block(self);
			datablock::itor block = tags_p->m_block;

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);

			VALUE arg = argv[0];
			if (TYPE(arg) == T_SYMBOL || TYPE(arg) == T_STRING)
			{
				const char* key;
				if (TYPE(arg) == T_SYMBOL)
					key = rb_id2name(rb_to_id(arg));
				else
                                    key = StringValuePtr(arg);

				if (block->type() == datablock::TYPE_REGION)
					if (!strcmp(key, "Terrain"))
						return make_str(block->terrainString());

                datakey::list_type &keys = block->data();
				for (datakey::itor itor = keys.begin(); itor != keys.end(); itor++)
					if (itor->key() == key)
					{
						if (itor->isInt())
							return INT2NUM(itor->getInt());
						else
							return make_str(itor->value());
					}

				return Qnil;
			}
			else if (TYPE(arg) == T_FIXNUM)
			{
				int index = FIX2INT(arg);

				if (block->type() == datablock::TYPE_REGION)
				{
					if (index == 0)
						return make_str(block->terrainString());
					else
						index -= 1;
				}
				if (index < 0 || (size_t)index >= block->data().size())
					return Qnil;

				datakey& tag = block->data()[index];
				if (tag.isInt())
					return INT2NUM(tag.getInt());
				else
					return make_str(tag.value());
			}

			// else
			rb_raise(rb_eTypeError, "wrong type of argument. Only numbers, strings and symbols are allowed.");
			return Qnil;
		}

	public:
		static Value klass;		// instance of the ruby class object

	protected:
		//filelist_t* m_files;
		//block_itor m_block, m_end;
		//connection_t m_signal;
	};

	/*static*/ Value Tags::klass;

	/******************************************
	* implementation of forward declared stuff
	******************************************/

	/*static*/ VALUE Block::makenew(VALUE klass, VALUE arg)
	{
		if (rb_obj_is_kind_of(arg, Tags::klass) == Qtrue)
		{
			Block* tags_p = get_block(arg);
			return bind_block(*tags_p->m_files, tags_p->m_block);
		}

		rb_raise(rb_eTypeError, "Block.new must be called with a Tags parameter.");
		return Qnil;
	}

	/***********************************
	* bind CR block to ruby class object
	***********************************/
	Value bind_block(filelist_t& files, block_itor first)
	{
		Block* block = new Block(files, first);

		return Value(Data_Wrap_Struct(Block::klass, Block::mark, Block::free, block));
	}

	Value bind_blocklist(filelist_t& files, block_itor first, const std::string& type)
	{
		BlockList* block = new BlockList(files, first, type);

		return Value(Data_Wrap_Struct(BlockList::klass, BlockList::mark, BlockList::free, block));
	}

	Value bind_tags(filelist_t& files, block_itor first)
	{
		Tags* tags = new Tags(files, first);

		return Value(Data_Wrap_Struct(Tags::klass, Tags::mark, Tags::free, tags));
	}

	void set_global(const std::string& name, Value value)
	{
		rb_gv_set(name.c_str(), value);
	}

	void define_ro_variable(const std::string& name, Value& value)
	{
		// binds a Value object to a global name
		rb_define_readonly_variable(name.c_str(), reinterpret_cast<VALUE*>(&value));
	}

	/***********************************
	* csmap utility functions for ruby
	***********************************/
	namespace
	{
		puts_handler_t puts_handler;
	}

	static VALUE local_puts(int argc, VALUE *argv, VALUE self)
	{
		for (int i = 0; i < argc; i++)
		{
			VALUE val(argv[i]);
			if (TYPE(val) == T_FIXNUM)
				puts_handler(FIX2INT(val));
			else if (TYPE(val) == T_ARRAY)
			{
				std::vector<value_t> array;

				VALUE elem;
				for (int i = 0; (elem = rb_ary_entry(val, i)) != Qnil; i++)
				{
					if (TYPE(elem) == T_FIXNUM)
						array.push_back(FIX2INT(elem));
					else
						array.push_back(Value(val).to_s());
				}

				puts_handler(array);
			}
			else if (val == Qnil)
				puts_handler(nil);
			else
				puts_handler(Value(val).to_s());
		}

		return Qnil;
	}

	// calls #inspect on each argument and forwards the result to #puts
	static VALUE local_p(int argc, VALUE *argv, VALUE self)
	{
		std::vector<VALUE> inspected(argc);

		for (int i = 0; i < argc; i++)
			inspected[i] = Ruby::Funcall(Value(argv[i]), "inspect");

		return local_puts(inspected.size(), &inspected[0], self);
	}

	static VALUE string_flatten(VALUE self)
	{
            VALUE s = StringValue(self);
            long length = RSTRING_LEN(s);
            const char* str = RSTRING_PTR(s);

		std::string flat = flatten(std::string(str, length));

		return rb_str_new(flat.c_str(), flat.size());
	}

	/***********************************
	* proc handling utilities
	***********************************/
	variant_t call_globalproc(const std::string& proc, puts_handler_t handler, Ruby::Value arg)
	{
		Value fun = Value(rb_gv_get(proc.c_str()));
		if (fun == Qnil)
			return nil;

		puts_handler_t orig_puts_handler = puts_handler;
		puts_handler = handler;

		try
		{
			Ruby::Value val = Ruby::Funcall(fun, "call", arg);
			puts_handler = orig_puts_handler;

			if (TYPE(val) == T_FIXNUM)
				return FIX2INT(val);
			else if (TYPE(val) == T_ARRAY)
			{
				std::vector<value_t> array;

				VALUE elem;
				for (int i = 0; (elem = rb_ary_entry(val, i)) != Qnil; i++)
				{
					if (TYPE(elem) == T_FIXNUM)
						array.push_back(FIX2INT(elem));
					else
						array.push_back(Value(val).to_s());
				}

				return array;
			}
			else if (val == Qnil)
				return nil;
			else if (val == Qtrue)
				return 1;
			else if (val == Qfalse)
				return 0;
			else
				return Value(val).to_s();
		}
		catch(...)
		{
			// set original puts-handler back and rethrow
			puts_handler = orig_puts_handler;
			throw;
		}
	}

	/***********************************
	* register ruby callback function
	***********************************/

	class Functor
	{
	public:
		Functor(method_t method) : method(method) {}

		static void mark(Functor* obj) { }
		static void free(Functor* obj) { delete obj; }

		static VALUE call(int argc, VALUE* argv)
		{
			//method(
			return INT2FIX(argc);
		}

	private:
		method_t method;
	};

	void register_module_method(const std::string& module, const std::string& name, method_t method)
	{
		Functor* func = new Functor(method);
		VALUE function = Data_Wrap_Struct(rb_cObject, Functor::mark, Functor::free, func);
		rb_define_global_const(module.c_str(), function);
		rb_define_singleton_method(function, name.c_str(), RUBY_METHOD_FUNC(&Functor::call), -1);
	}

	/***********************************
	* initialize ruby <-> csmap bindings
	***********************************/
	void puts_handler_ingore(const variant_t&)
	{}

	bool initialize()
	{
		// set default puts_handler
		puts_handler = &puts_handler_ingore;

		// there is no stdout.
		Ruby::DefineMethod(Value(rb_mKernel), "puts", &local_puts, -1);
		Ruby::DefineMethod(Value(rb_mKernel), "p", &local_p, -1);

		// id should return the CR id instead.
		rb_undef_method(rb_cObject, "id");

		// define flatten method for strings
		Ruby::DefineMethod(Value(rb_cString), "flatten", &string_flatten, 0);

		// define Block class
		Block::klass = Ruby::DefineClass("Block");
		rb_define_singleton_method(Block::klass, "new", RUBY_METHOD_FUNC(Block::makenew), 1);
		Ruby::DefineMethod(Block::klass, "to_s", &Block::to_s, 0);
		Ruby::DefineMethod(Block::klass, "tags", &Block::tags, 0);
		Ruby::DefineMethod(Block::klass, "blocks", &Block::blocks, 0);
		Ruby::DefineMethod(Block::klass, "type", &Block::type, 0);
		Ruby::DefineMethod(Block::klass, "id", &Block::id, 0);
		Ruby::DefineMethod(Block::klass, "id36", &Block::id36, 0);
		Ruby::DefineMethod(Block::klass, "terrain", &Block::terrain, 0);
		
		// define BlockList class
		BlockList::klass = Ruby::DefineClass("BlockList");
		rb_define_singleton_method(BlockList::klass, "new", RUBY_METHOD_FUNC(BlockList::makenew), 1);
		Ruby::DefineMethod(BlockList::klass, "each", &BlockList::each, 0);
		rb_include_module(BlockList::klass, rb_mEnumerable);
		Ruby::DefineMethod(BlockList::klass, "size", &BlockList::size, 0);
		Ruby::DefineMethod(BlockList::klass, "[]", &BlockList::index_op, -1);

		// define Tags class
		Tags::klass = Ruby::DefineClass("Tags");
		rb_define_singleton_method(Tags::klass, "new", RUBY_METHOD_FUNC(Tags::makenew), 1);
		Ruby::DefineMethod(Tags::klass, "each", &Tags::each, 0);
		rb_include_module(Tags::klass, rb_mEnumerable);
		Ruby::DefineMethod(Tags::klass, "each_pair", &Tags::each_pair, 0);
		Ruby::DefineMethod(Tags::klass, "each_key", &Tags::each_key, 0);
		Ruby::DefineMethod(Tags::klass, "each_value", &Tags::each_value, 0);
		Ruby::DefineMethod(Tags::klass, "keys", &Tags::keys, 0);
		Ruby::DefineMethod(Tags::klass, "values", &Tags::values, 0);
		Ruby::DefineMethod(Tags::klass, "size", &Tags::size, 0);
		Ruby::DefineMethod(Tags::klass, "[]", &Tags::index_op, -1);
		return true;
	}

}	// ::bindings
