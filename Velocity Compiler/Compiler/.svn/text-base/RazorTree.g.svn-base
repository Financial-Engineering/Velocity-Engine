tree grammar RazorTree;

options{
	tokenVocab = Razor;
	ASTLabelType = CommonTree;
	output = template;
}

@header {
	import java.util.HashMap;
	import java.util.HashSet;
	import java.io.*;
}

@members {
	public static class Extern {
	    public String alias;
	    public String value;
	    public Extern(String _alias, String _value) {
	        alias=_alias; 
    	        value=_value;
	    }
	}
	
	void addExtern(String var, String alias, StringTemplate init) {
	    RazorTree.Extern ext = new RazorTree.Extern(alias!=null?alias:var,
	                                                      init!=null?init.toString():null); 
	    $program::externs.put(var, ext);
	}
	
	void addGlobal(String var, StringTemplate init) {
	    $program::globals.put(var, init!=null?init.toString():null);
	}
	
	void addLocal(String var) {
	    if(!$funDecl::params.contains(var))
	        $funDecl::locals.add(var);
	}
}

program[HashMap<String,Enum> symbols]
scope {
    HashMap usings;
    HashMap specfns;
    HashMap<String,Extern> externs;
    HashMap globals;
}
@init {
    $program::usings = new HashMap();
    $program::specfns = new HashMap();
    $program::externs = new HashMap<String,Extern>();
    $program::globals = new HashMap();
}		
	: ^(PROGRAM ID usingDecl* extVarDecl* stmtList? funs+=funDecl+)
		-> program(template={$ID.text},usings={$program::usings},externs={$program::externs},
		           globals={$program::globals},stats={$stmtList.st},
		           functions={$funs},
		           price={$program::specfns.get("price")},
		           payoff={$program::specfns.get("payoff")})
;

usingDecl
	:^(USING val1=ID val2=ID?) {$program::usings.put($val1.text, $val2.text);}
	;

extVarDecl
	:^(EXTERN ID ^(ALIAS sl=STRING_LITERAL?) varInit?)
	{addExtern($ID.text,$sl.text,$varInit.st);}
	;

varInit
	: num -> {$num.st}
	| vector -> {$vector.st}
	;
	
			
funDecl 
scope {
   HashSet params;
   HashSet locals;
}
@init {
   $funDecl::params = new HashSet();
   $funDecl::locals = new HashSet();
}
	: ^(FUNCTION name=ID ^(ATTRIB attrib=ID?) ^(RETVALS r+=param*) ^(PARAMS p+=param*) stmtList)
	  { $program::specfns.put($attrib.text,$name.text);}
	  -> function(name={$name.text},args={$p},rets={$r},locals={$funDecl::locals},stats={$stmtList.st})
;

param 	: ^(LOCAL ID) {$funDecl::params.add($ID.text);} -> {%{$ID.text}}	
	;
	
stmtList returns [ArrayList localVars]
:	^(STMTS stats+=stmt*) -> statementList(stats={$stats})
;

stmt
	:	
		  RETURN -> {%{"return;"}}
		| BREAK -> {%{"break;"}}
		| CONTINUE -> {%{"continue;"}}
//		| ^(FOR ^(LOCAL var=ID) e1=expr e2=expr e3=expr? stmtList)
		| ^(FOR ^(LOCAL var=ID) ^(SEQUENCE e1=expr e2=expr e3=expr?) stmtList)
		 	{$funDecl::params.add($ID.text);}
			-> for(var={$var},e1={$e1.st},e2={$e2.st},e3={$e3.st},stats={$stmtList.st})
		| ^(WHILE expr stmtList) 
			-> while(e1={$expr.st},s1={$stmtList.st})
		| ^(IF expr stats1=stmtList stats2=stmtList?)
			 -> if(e1={$expr.st},s1={$stats1.st},s2={$stats2.st})
		| fncall -> {$fncall.st}
		| ^(GLOBAL ID varInit?) {addGlobal($ID.text,$varInit.st);}
		| ^('=' varref expr) -> assign(id={$varref.st},e1={$expr.st})
	;

//argList	:	^(ARGS e1+=expr*) -> {$e1.st}
//	;

expr
	:	^('||' e1=expr e2=expr) -> or(e1={$e1.st},e2={$e2.st})
		| ^('&&' e1=expr e2=expr) -> and(e1={$e1.st},e2={$e2.st})
		| ^('==' e1=expr e2=expr) -> eq(e1={$e1.st},e2={$e2.st})
		| ^('~=' e1=expr e2=expr) -> neq(e1={$e1.st},e2={$e2.st})
		| ^('<' e1=expr e2=expr) -> lt(e1={$e1.st},e2={$e2.st})
		| ^('>' e1=expr e2=expr) -> gt(e1={$e1.st},e2={$e2.st})
		| ^('<=' e1=expr e2=expr)-> lte(e1={$e1.st},e2={$e2.st})
		| ^('>=' e1=expr e2=expr)-> gte(e1={$e1.st},e2={$e2.st})
		| ^('+' e1=expr e2=expr) -> add(e1={$e1.st},e2={$e2.st})
		| ^('-' e1=expr e2=expr) -> sub(e1={$e1.st},e2={$e2.st})
		| ^('*' e1=expr e2=expr) -> mul(e1={$e1.st},e2={$e2.st})
		| ^('.*' e1=expr e2=expr) -> vmul(e1={$e1.st},e2={$e2.st})
		| ^('/' e1=expr e2=expr) -> div(e1={$e1.st},e2={$e2.st})
		| ^('./' e1=expr e2=expr) -> vdiv(e1={$e1.st},e2={$e2.st})
		| ^('\\' e1=expr e2=expr) -> rdiv(e1={$e1.st},e2={$e2.st})
		| ^('^' e1=expr e2=expr) -> pow(e1={$e1.st},e2={$e2.st})
		| ^('.^' e1=expr e2=expr) -> vpow(e1={$e1.st},e2={$e2.st})
		| ^(NOT e1=expr) -> not(e1={$e1.st})
		| ^(NEGATE e1=expr) -> neg(e1={$e1.st})
		| atom -> {$atom.st}
;

atom
	:	fncall -> {$fncall.st}
		| varref -> {$varref.st}
		| num -> {$num.st}
		| sequence -> {$sequence.st}
		| vector -> {$vector.st}
;

varref
	: ^(EXTERN ID expr?) -> extvar(var={$ID.text}, idx={$expr.st})
	| ^(GLOBAL ID expr?) -> globalvar(var={$ID.text}, idx={$expr.st})
	| ^(LOCAL ID expr?) {addLocal($ID.text);}
 			     -> locvar(var={$ID.text}, idx={$expr.st})
	;

fncall
	:
	^(CALL ID ^(ARGS exprs+=expr*))-> funcall(name={$ID.text},exprs={$exprs})	
	;

method
	:
	^(METHOD ID ^(ARGS exprs+=expr*)) -> methodcall(name={$ID.text},exprs={$exprs})	
	;
	
sequence
	: ^(SEQUENCE e1=expr e2=expr e3=expr?) -> sequence(e1={$e1.st},e2={$e2.st},e3={$e3.st})
	;


vector
	: ^(VECTOR vectElements) -> {$vectElements.st}
	;
	
vectElements
	: (n+=num|n+=varref)* -> vector(seq={$n})
	;

num	: ^(NUM INT) -> {%{$INT.text}}
	| ^(NUM FLOAT) -> {%{$FLOAT.text}}
	;

