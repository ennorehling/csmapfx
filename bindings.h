#ifndef CSMAP_BINDINGS_H_
#define CSMAP_BINDINGS_H_

#include <list>
#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/variant.hpp>

#include "datafile.h"
#include "csruby.h"

namespace bindings
{
	// types
	typedef std::list<datafile> filelist_t;
	typedef datablock::itor block_itor;

	// initialize ruby <-> csmap bindings
    bool initialize();

	// binds CR block to a ruby object
	Ruby::Value bind_block(filelist_t& files, block_itor first);
	Ruby::Value bind_blocklist(filelist_t& files, block_itor first, const std::string& type = "");
	Ruby::Value bind_tags(filelist_t& files, block_itor first);

	void set_global(const std::string& name, Ruby::Value value);
	void define_ro_variable(const std::string& name, Ruby::Value& value);		

	// puts handler function
	struct nil_t {};
	extern nil_t nil;
	typedef boost::variant<nil_t, int, std::string> value_t;
	typedef boost::variant<nil_t, int, std::vector<value_t>, std::string> variant_t;
	typedef boost::function<void(const variant_t&)> puts_handler_t;	// Ruby's #puts calls the handler on each line

	// calls the proc stored in the global variable named 'proc' while setting the puts_handler
	// e.g.: call_globalproc("$getinfos", &addInfo);
	variant_t call_globalproc(const std::string& proc, puts_handler_t handler, Ruby::Value = Ruby::Value());

	// register callback
	typedef boost::function<value_t(const variant_t&)> method_t;
	void register_module_method(const std::string& module, const std::string& name, method_t method);
}

#endif // CSMAP_BINDINGS_H_
