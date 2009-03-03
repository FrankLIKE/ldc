#ifndef LDC_GEN_FUNCTIONS_H
#define LDC_GEN_FUNCTIONS_H

const llvm::FunctionType* DtoFunctionType(Type* t, Type* thistype, Type* nesttype, bool ismain = false);
const llvm::FunctionType* DtoFunctionType(FuncDeclaration* fdecl);

const llvm::FunctionType* DtoBaseFunctionType(FuncDeclaration* fdecl);

void DtoResolveFunction(FuncDeclaration* fdecl);
void DtoDeclareFunction(FuncDeclaration* fdecl);
void DtoDefineFunction(FuncDeclaration* fd);

void DtoDefineNakedFunction(FuncDeclaration* fd);
void emitABIReturnAsmStmt(IRAsmBlock* asmblock, Loc loc, FuncDeclaration* fdecl);

DValue* DtoArgument(Argument* fnarg, Expression* argexp);
void DtoVariadicArgument(Expression* argexp, LLValue* dst);

#endif
