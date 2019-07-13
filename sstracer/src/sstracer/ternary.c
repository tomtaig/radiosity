#include "ternary.h"

//
// TernaryTraverse
//
tern_t *TernaryTraverse(tern_t *tree, int *num, char *match) {

	// Find and return the n'th string in the tree

	tern_t	*n;

	if(!tree) {
		return tree;
	}

	if(tree->in && *num<=0) {
		*match=tree->eqv;
		match++;
		*match='\0';
		return tree;
	}

	if(tree->in) {
		(*num)--;
	}

	n=TernaryTraverse(tree->lo, num, match);
	
	if(tree->eq && !n) {
		*match=tree->eqv;
		n=TernaryTraverse(tree->eq, num, match+1);
	}

	if(n) {
		return n;
	}

	return TernaryTraverse(tree->hi, num, match);
}

//
// TernaryPrefix
//
tern_t *TernaryPrefix(tern_t *tree, char *str, int *num, char *match) {

	// Read str and find the place for the traverse

	if(!tree) {
		return NULL;
	}

	if(*str=='\0') {
		return TernaryTraverse(tree, num, match);
	}

	if(*str>tree->eqv) {
		return TernaryPrefix(tree->hi, str, num, match);
	}

	if(*str<tree->eqv) {
		return TernaryPrefix(tree->lo, str, num, match);
	}

	*match=tree->eqv;

	if(tree->in && !(*num) && *(str+1)=='\0') {
		*(++match)='\0';
		return tree;
	}

	if(tree->in && *(str+1)=='\0') {
		(*num)--;
	}

	return TernaryPrefix(tree->eq, str+1, num, match+1);
}

//
// TernarySearch
//
tern_t *TernarySearch(tern_t *tree, char *str) {

	if(!tree) {
		return tree;
	}

	if(*str>tree->eqv) {
		return TernarySearch(tree->hi, str);
	}

	if(*str<tree->eqv) {
		return TernarySearch(tree->lo, str);
	}

	if(*(str+1)=='\0') {
		return tree;
	}

	return TernarySearch(tree->eq, str+1);
}

//
// TernaryString
//
tern_t *TernaryString(tern_t *tree, char *str, void *value) {

	if(*str=='\0') {
		return NULL;
	}

	if(!tree) {
		tree=malloc(sizeof(tern_t));
		tree->eqv=*str;
		tree->shr=0;
		tree->hi=NULL;
		tree->lo=NULL;
		tree->eq=NULL;
		tree->in=(*(str+1)=='\0')?value:NULL;
	}
	
	if(*str>tree->eqv) {
		tree->hi=TernaryString(tree->hi, str, value);
	}else
	if(*str<tree->eqv) {
		tree->lo=TernaryString(tree->lo, str, value);
	}else{
		tree->eq=TernaryString(tree->eq, str+1, value);
		tree->shr++;
	}

	return tree;
}

//
// TernaryRemove
//
tern_t *TernaryRemove(tern_t *tree, char *str) {

	tern_t	*n;

	if(*str=='\0' || !tree) {
		return tree;
	}

	if(*str>tree->eqv) {
		tree->hi=TernaryRemove(tree->hi, str);
	}else
	if(*str<tree->eqv) {
		tree->lo=TernaryRemove(tree->lo, str);
	}else{
		tree->eq=TernaryRemove(tree->eq, str+1);
		tree->shr--;
	}
	
	if(tree->eqv==*str && *(str+1)=='\0') {
		tree->in=NULL;
	}

	if(tree->shr) {
		return tree;
	}

	if(tree->lo && !tree->hi) {
		n=tree->lo;
		memcpy(tree, tree->lo, sizeof(tern_t));
		free(n);
	}else
	if(tree->hi && !tree->lo) {
		n=tree->hi;
		memcpy(tree, tree->hi, sizeof(tern_t));
		free(n);
	}else
	if(!tree->lo && !tree->hi) {
		free(tree);
		tree=NULL;
	}
	
	return tree;
}

//
// TernaryDestroy
//
void TernaryDestroy(tern_t *tree) {

	if(!tree) {
		return;
	}

	TernaryDestroy(tree->hi);
	TernaryDestroy(tree->lo);
	TernaryDestroy(tree->eq);

	free(tree);

	return;
}