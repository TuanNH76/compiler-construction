/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object *obj);
void freeScope(Scope *scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab *symtab;
Type *intType;
Type *charType;

/******************* Type utilities ******************************/

Type *makeIntType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type *makeCharType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type *makeArrayType(int arraySize, Type *elementType)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type *duplicateType(Type *type)
{
  // TODO
  Type *dupType = (Type *)malloc(sizeof(Type));
  dupType->typeClass = type->typeClass;
  dupType->arraySize = type->arraySize;
  if (type->elementType != NULL)
    dupType->elementType = duplicateType(type->elementType);
  return dupType;
}

int compareType(Type *type1, Type *type2)
{
  // TODO
  if (type1->typeClass != type2->typeClass)
    return 0;
  if (type1->typeClass == TP_ARRAY)
  {
    if (type1->arraySize != type2->arraySize)
      return 0;
    return compareType(type1->elementType, type2->elementType);
  }
  return 1;
}

void freeType(Type *type)
{
  // TODO
  if (type->typeClass == TP_ARRAY)
  {
    freeType(type->elementType);
    free(type);
  }
  else
  {
    free(type);
  }
}

/******************* Constant utility ******************************/

ConstantValue *makeIntConstant(int i)
{
  // TODO
  ConstantValue *intConst = (ConstantValue *)malloc(sizeof(ConstantValue));
  intConst->type = TP_INT;
  intConst->intValue = i;
  return intConst;
}

ConstantValue *makeCharConstant(char ch)
{
  // TODO
  ConstantValue *charConst = (ConstantValue *)malloc(sizeof(ConstantValue));
  charConst->type = TP_CHAR;
  charConst->charValue = ch;
  return charConst;
}

ConstantValue *duplicateConstantValue(ConstantValue *v)
{
  // TODO
  ConstantValue *dupConst = (ConstantValue *)malloc(sizeof(ConstantValue));
  dupConst->type = v->type;
  if (v->type == TP_INT)
    dupConst->intValue = v->intValue;
  else if (v->type == TP_CHAR)
    dupConst->charValue = v->charValue;
  return dupConst;
}

/******************* Object utilities ******************************/

Scope *createScope(Object *owner, Scope *outer)
{
  Scope *scope = (Scope *)malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object *createProgramObject(char *programName)
{
  Object *program = (Object *)malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes *)malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program, NULL);
  symtab->program = program;

  return program;
}

Object *createConstantObject(char *name)
{
  // TODO
  Object *constant = (Object *)malloc(sizeof(Object));
  strcpy(constant->name, name);
  constant->kind = OBJ_CONSTANT;
  constant->constAttrs = (ConstantAttributes *)malloc(sizeof(ConstantAttributes));
  // Value
  constant->constAttrs->value = (ConstantValue *)malloc(sizeof(ConstantValue));
  return constant;
}

Object *createTypeObject(char *name)
{
  // TODO
  Object *type = (Object *)malloc(sizeof(Object));
  strcpy(type->name, name);
  type->kind = OBJ_TYPE;
  type->typeAttrs = (TypeAttributes *)malloc(sizeof(TypeAttributes));
  // Value
  type->typeAttrs->actualType = (Type *)malloc(sizeof(Type));
  return type;
}

Object *createVariableObject(char *name)
{
  // TODO
  Object *variable = (Object *)malloc(sizeof(Object));
  strcpy(variable->name, name);
  variable->kind = OBJ_VARIABLE;
  variable->varAttrs = (VariableAttributes *)malloc(sizeof(VariableAttributes));
  variable->varAttrs->scope = symtab->currentScope;
  return variable;
}

Object *createFunctionObject(char *name)
{
  // TODO
  Object *function = (Object *)malloc(sizeof(Object));
  strcpy(function->name, name);
  function->kind = OBJ_FUNCTION;
  function->funcAttrs = (FunctionAttributes *)malloc(sizeof(FunctionAttributes));
  function->funcAttrs->scope = createScope(function, symtab->currentScope);
  function->funcAttrs->paramList = (ObjectNode *)malloc(sizeof(ObjectNode));
  function->funcAttrs->returnType = (Type *)malloc(sizeof(Type));
  function->funcAttrs->scope = createScope(function, symtab->currentScope);
  return function;
}

Object *createProcedureObject(char *name)
{
  // TODO
  Object *procedure = (Object *)malloc(sizeof(Object));
  procedure->kind = OBJ_PROCEDURE;
  procedure->procAttrs = (ProcedureAttributes *)malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->paramList = (ObjectNode *)malloc(sizeof(ObjectNode));
  procedure->procAttrs->scope = createScope(procedure, symtab->currentScope);

  return procedure;
}

Object *createParameterObject(char *name, enum ParamKind kind, Object *owner)
{
  // TODO
  Object *parameter = (Object *)malloc(sizeof(Object));
  strcpy(parameter->name, name);
  parameter->kind = OBJ_PARAMETER;
  parameter->paramAttrs = (ParameterAttributes *)malloc(sizeof(ParameterAttributes));
  parameter->paramAttrs->kind = kind;
  parameter->paramAttrs->type = (Type *)malloc(sizeof(Type));

  return parameter;
}

void freeObject(Object* obj) {
  switch (obj->kind) {
  case OBJ_CONSTANT:
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
  case OBJ_TYPE:
    free(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
  case OBJ_VARIABLE:
    free(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  case OBJ_FUNCTION:
    freeReferenceList(obj->funcAttrs->paramList);
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    free(obj->funcAttrs);
    break;
  case OBJ_PROCEDURE:
    freeReferenceList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
  case OBJ_PROGRAM:
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
  case OBJ_PARAMETER:
    freeType(obj->paramAttrs->type);
    free(obj->paramAttrs);
  }
  free(obj);
}

void freeScope(Scope *scope)
{
  // TODO
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList)
{
  // TODO
  ObjectNode *list = objList;

  while (list != NULL)
  {
    ObjectNode *node = list;
    list = list->next;
    freeObject(node->object);
    free(node);
  }
}

void freeReferenceList(ObjectNode *objList)
{
  // TODO
  ObjectNode *list = objList;

  while (list != NULL)
  {
    ObjectNode *node = list;
    list = list->next;
    free(node);
  }
}

void AddObject(ObjectNode **objList, Object *obj)
{
  ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL)
    *objList = node;
  else
  {
    ObjectNode *n = *objList;
    while (n->next != NULL)
      n = n->next;
    n->next = node;
  }
}

Object *findObject(ObjectNode *objList, char *name)
{
  // TODO
  while (objList != NULL)
  {
    if (strcmp(objList->object->name, name) == 0)
      return objList->object;
    else
      objList = objList->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void)
{
  Object *obj;
  Object *param;

  symtab = (SymTab *)malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;

  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  AddObject(&(obj->procAttrs->paramList), param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  AddObject(&(obj->procAttrs->paramList), param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  AddObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void)
{
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope *scope)
{
  symtab->currentScope = scope;
}

void exitBlock(void)
{
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object *obj)
{
  if (obj->kind == OBJ_PARAMETER)
  {
    Object *owner = symtab->currentScope->owner;
    switch (owner->kind)
    {
    case OBJ_FUNCTION:
      AddObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      AddObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }

  AddObject(&(symtab->currentScope->objList), obj);
}