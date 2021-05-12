#ifndef CRONA_SYMBOL_TABLE_HPP
#define CRONA_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include "types.hpp"

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V> using HashMap = std::unordered_map<K, V>;

namespace crona
{
//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols
// exist for the lifetime of a scope in the
// symbol table.
class SemSymbol {
    public:
	enum Kind { VAR = 0, FN };
	SemSymbol(Kind kind, std::string id, DataType *type)
		: _kind(kind), _id(id), _type(type)
	{
	}
	virtual std::string typeString() const
	{
		return _type->getString();
	};
	std::string kind() const
	{
		switch (_kind) {
		case Kind::VAR:
			return "var";
		case Kind::FN:
			return "fn";
		default:
			return "err_kind";
		}
	}
	virtual DataType *type() const
	{
		return _type;
	}
	std::string ID() const
	{
		return _id;
	}

    protected:
	Kind _kind;
	std::string _id;
	DataType *_type;
};

//A single scope. The symbol table is broken down into a
// chain of scope tables, and each scope table holds
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
    public:
	ScopeTable()
	{
	}
	SemSymbol *getSymbol(std::string id)
	{
		if (contains(id))
			return symbols[id];
		return nullptr;
	}
	bool contains(std::string id) const
	{
		return symbols.find(id) != symbols.end();
	}
	bool insert(std::string id, SemSymbol *sem)
	{
		if (contains(id))
			return false;
		symbols[id] = sem;
		return true;
	}

    private:
	HashMap<std::string, SemSymbol *> symbols;
};

class SymbolTable {
    public:
	SymbolTable()
	{
		enter(); // add global scope table
	}
	void enter()
	{
		// enter new scope
		scopeTableChain.push_front(new ScopeTable());
	}
	void leave()
	{
		// leave current scope
		if (!scopeTableChain.empty())
			scopeTableChain.pop_front();
	}
	bool contains(std::string id) const
	{
		for (auto scope : scopeTableChain)
			if (scope->contains(id))
				return true;
		return false;
	}
	bool insert(std::string id, SemSymbol *sem)
	{
		if (scopeTableChain.empty())
			return false;
		return currentScope()->insert(id, sem);
	}
	SemSymbol *getSymbol(std::string id)
	{
		for (auto scope : scopeTableChain)
			if (auto sem = scope->getSymbol(id))
				return sem;
		return nullptr;
	}
	ScopeTable *currentScope()
	{
		if (scopeTableChain.empty())
			return nullptr;
		return scopeTableChain.front();
	}
	//TODO: add functions to create a new ScopeTable
	// when a new scope is entered, drop a ScopeTable
	// when a scope is exited, etc.

    private:
	std::list<ScopeTable *> scopeTableChain;
};

} // namespace crona

#endif
