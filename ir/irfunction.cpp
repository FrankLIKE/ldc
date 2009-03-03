
#include "gen/llvm.h"
#include "gen/tollvm.h"
#include "gen/abi.h"
#include "ir/irfunction.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

IrFuncTyArg::IrFuncTyArg(Type* t, bool bref, unsigned a)
{
    type = t;
    ltype = bref ? DtoType(t->pointerTo()) : DtoType(t);
    attrs = a;
    byref = bref;
    rewrite = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

llvm::Value* IrFuncTy::putRet(Type* dty, llvm::Value* val)
{
    assert(!arg_sret);
    if (ret->rewrite)
        return ret->rewrite->put(dty, val);
    return val;
}

llvm::Value* IrFuncTy::getRet(Type* dty, llvm::Value* val)
{
    assert(!arg_sret);
    if (ret->rewrite)
        return ret->rewrite->get(dty, val);
    return val;
}

llvm::Value* IrFuncTy::putParam(Type* dty, int idx, llvm::Value* val)
{
    assert(idx >= 0 && idx < args.size() && "invalid putParam");
    if (args[idx]->rewrite)
        return args[idx]->rewrite->put(dty, val);
    return val;
}

llvm::Value* IrFuncTy::getParam(Type* dty, int idx, llvm::Value* val)
{
    assert(idx >= 0 && idx < args.size() && "invalid getParam");
    if (args[idx]->rewrite)
        return args[idx]->rewrite->get(dty, val);
    return val;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

IrFunction::IrFunction(FuncDeclaration* fd)
{
    decl = fd;

    Type* t = fd->type->toBasetype();
    assert(t->ty == Tfunction);
    type = (TypeFunction*)t;
    func = NULL;
    allocapoint = NULL;

    queued = false;
    defined = false;

    retArg = NULL;
    thisArg = NULL;
    nestArg = NULL;

    nestedVar = NULL;
    
    _arguments = NULL;
    _argptr = NULL;
    
    nextUnique.push(0);
}

std::string IrFunction::getScopedLabelName(const char* ident)
{
    if(labelScopes.empty())
        return std::string(ident);

    std::string result = "__";
    for(unsigned int i = 0; i < labelScopes.size(); ++i)
        result += labelScopes[i] + "_";
    return result + ident;
}

void IrFunction::pushUniqueLabelScope(const char* name)
{
    std::ostringstream uniquename;
    uniquename << name << nextUnique.top()++;
    nextUnique.push(0);
    labelScopes.push_back(uniquename.str());
}

void IrFunction::popLabelScope()
{
    labelScopes.pop_back();
    nextUnique.pop();
}

void IrFunction::setNeverInline()
{
    assert(!func->hasFnAttr(llvm::Attribute::AlwaysInline) && "function can't be never- and always-inline at the same time");
    func->addFnAttr(llvm::Attribute::NoInline);
}

void IrFunction::setAlwaysInline()
{
    assert(!func->hasFnAttr(llvm::Attribute::NoInline) && "function can't be never- and always-inline at the same time");
    func->addFnAttr(llvm::Attribute::AlwaysInline);
}
