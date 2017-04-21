grammar Razor;

options{
	output = AST;
	ASTLabelType = CommonTree;
	//tokenVocab = RazorLexer;
}

tokens{
	PROGRAM; LOCAL; CALL; ARGS; RETVALS; VAR; NUM; VECTOR; STMTS; SEQUENCE; METHOD;
	NOT; NEGATE; RETURN; BREAK; CONTINUE; FOR; WHILE; USING; ALIAS; ATTRIB; PARAMS;	
}

scope RScope {
	String name;
	HashMap<String,Enum> symbols;
}

@header {
    import java.util.Map;
    import java.util.HashMap;
}

@members {
    // should be part of RScope but somehow fails to compile
    enum Symbol {
        LOCAL,
        GLOBAL,
        EXTERN,
        FUNCTION,
        METHOD
    };

    Symbol getSymbol(String id) {
        for (int s=$RScope.size()-1; s>=0; s--) {
            if ($RScope[s]::symbols.containsKey(id)) {
            	return (Symbol)($RScope[s]::symbols.get(id));
            }
        }
        return null;
    }
    
    boolean isDefined(String id) {
        for (int s=$RScope.size()-1; s>=0; s--) {
            if ($RScope[s]::symbols.containsKey(id)) {
            	return true;
            }
        }
        return false;
    }
    
    boolean isFunction(String id) {
        for (int s=$RScope.size()-1; s>=0; s--) {
            Symbol val=(Symbol)$RScope[s]::symbols.get(id);
            if ((val !=null) && (val == Symbol.FUNCTION)) {
            	return true;
            }
        }
        return false;
    }
    
    boolean isMethod(String id) {
        for (int s=$RScope.size()-1; s>=0; s--) {
            Symbol val=(Symbol)$RScope[s]::symbols.get(id);
            if ((val !=null) && (val == Symbol.METHOD)) {
            	return true;
            }
        }
        return false;
    }
    
    boolean isVariable(String id) {
        for (int s=$RScope.size()-1; s>=0; s--) {
            Symbol  val=(Symbol)$RScope[s]::symbols.get(id);
            if ((val !=null) && ((val == Symbol.LOCAL) || (val == Symbol.GLOBAL) || (val == Symbol.EXTERN))) {
            	return true;
            }
        }
        return false;
    }

    boolean isExtern(String sym) {
    	return "EXTERN".equals(sym);
    }
    
    boolean isLocal(String sym) {
    	return "LOCAL".equals(sym);
    }
    
    boolean isGlobal(String sym) {
    	return "GLOBAL".equals(sym);
    }
    
    void addFunction(String id) {
    
        // Only insert function names at function scope
        if ($RScope.size() > 1)
        	$RScope[1]::name = id;
        	
	$RScope[0]::symbols.put(id,Symbol.FUNCTION);
    }

    void addMethod(String id) {
	$RScope[0]::symbols.put(id,Symbol.METHOD);
    }
      
    // externs are level 0, globals level 1, locals level 2 scope
    Symbol addLocal(String id, boolean overwrite) {
    	Symbol sym = getSymbol(id);
    	if (overwrite || sym == null) {
    	   sym = Symbol.LOCAL;
	   $RScope::symbols.put(id,sym);
	}
	return sym;
    }

    Symbol addLocal(String id) {
	return addLocal(id,false);
    }
    
    Symbol addVariable(String id, Symbol sym)
    {
    	Symbol s = getSymbol(id);
    	if (s == null) {
    	   s = sym;
	   $RScope[0]::symbols.put(id,s);
	}    
    	return s;
    }
    
    // externs are level 0, globals level 1, locals level 2 scope
    Symbol addGlobal(String id) {
	return addVariable(id, Symbol.GLOBAL);
    }

    Symbol addExtern(String id) {
	return addVariable(id, Symbol.EXTERN);
    }

    // Globals are at level 1 iff level 1 is not a function
    
    List getVariables(int level,Symbol sym) {
        ArrayList syms = null;
        HashMap<String,Enum> h = $RScope[1]::symbols;
    	if (h.containsValue(sym)) {
            syms = new ArrayList();
            for (Map.Entry<String, Enum> entry : h.entrySet()) {
               if (entry.getValue() == sym) syms.add(entry.getKey());
            } 
	}
        return syms;	
    }

    List getExterns() {
        return getVariables(0,Symbol.EXTERN);
    }
    
    List getGlobals() {
        return getVariables(0,Symbol.GLOBAL);
    }
    
    List getLocals() {
        ArrayList syms = null;
        if ($RScope.size() > 2) {
	        HashMap<String,Enum> h = $RScope::symbols;
	    	if (h.containsValue(Symbol.LOCAL)) {
	            syms = new ArrayList();
	            for (Map.Entry<String, Enum> entry : h.entrySet()) {
	               if (entry.getValue() == Symbol.LOCAL) syms.add(entry.getKey());
	            } 
		}
	}
        return syms;	    
    }
    
    String nextToken() {
    	return input.LT(1).getText();
    }
}

program returns [String name, HashMap<String,Enum> symbols]
scope RScope;
@init{
   $RScope::symbols = new HashMap();
   
   // predefined math functions
  addFunction("min");  addFunction("max");  addFunction("sum");  addFunction("abs");
  addFunction("avga"); addFunction("avgg");  addFunction("log");  addFunction("exp");
  addFunction("sin");  addFunction("cos");  addFunction("tan");  addFunction("sqrt");  
  addFunction("factorial");
  
  // statistical functions
  addFunction("cov");    addFunction("mean");   addFunction("median"); addFunction("mode");
  addFunction("std");    addFunction("var");
  
  // statistical distributions
  addFunction("lognormal"); addFunction("gaussian");
  
  // statistical distributions
  addFunction("blackProcess"); addFunction("black2DProcess");
  
  // predefined vector functions	
  addFunction("isempty");  addFunction("ones");    addFunction("zeros");

  // predefined rounding/remainder functions	
  addFunction("ceil"); addFunction("floor");   addFunction("mod");
  
  // special functions that act as methods
  addMethod("size");
  
}
@after{
	$name = $RScope::name;
	$symbols = $RScope::symbols;
}
	: template usingList extVarList stmtList funcList
	-> ^(PROGRAM template usingList? extVarList? stmtList? funcList)
	;
	
template
	:  TEMPLATE ID {$RScope::name=$ID.text;} EOS -> ID
;

usingList
	: usingDecl*
	;

usingDecl
	: USING ns=ID
	( '::' m=ID -> ^(USING $ns $m)
	|           -> ^(USING $ns)
	) EOS
	;

extVarList
	:	extVarDecl*
	;
	
extVarDecl
	:  EXTERN ID ('(' STRING_LITERAL ')')? ('=' varInit)? EOS
	 {addExtern($ID.text);}
	 -> ^(EXTERN ID ^(ALIAS STRING_LITERAL?) varInit?)
	;

varInit
	: num | vector
	;

funcList
	:	funDecl*
;

funDecl
scope RScope;
@init{
	$RScope::symbols=new HashMap();
}
	: attrib? FUNCTION retList? ID paramList? {addFunction($ID.text);} stmtList END 
	  -> ^(FUNCTION ID ^(ATTRIB attrib?) retList? paramList? stmtList)
;

// for now functions can have only 1 attribute
attrib options { k=1;}
	:	'[' ID ']' -> ID+
	;
	
retList 
	: '[' (param (',' param)*)? ']' '=' -> ^(RETVALS param*)
	;
	
paramList 
	:'(' (param (',' param)*)? ')' -> ^(PARAMS param*)
	;

param
	: ID {addLocal($ID.text,true);} -> ^(LOCAL ID)
	;
		
stmtList
scope RScope;
@init{
	$RScope::symbols=new HashMap();
}
	: stmt* -> ^(STMTS stmt*)
	;

stmt
	: EOS!
	| RETURN
	| BREAK
	| CONTINUE
	| forStmt
	| whileStmt
	| ifStmt
	| fncall
	| globalDecl
	| varref '='^ expr
	;

globalDecl
	:  GLOBAL ID ('=' varInit)? EOS
	 {addGlobal($ID.text);}
	 -> ^(GLOBAL ID varInit?)
	;

loopStmt
	:	whileStmt
	|	forStmt
	;
	
whileStmt
	:
	WHILE expr stmtList END -> ^(WHILE expr stmtList)
	;
			
//forStmt
//	:
//	FOR ID '=' e1=expr ':' e2=expr {addLocal($ID.text);}
//	( ':' e3=expr stmtList -> ^(FOR ^(LOCAL ID)  $e1 $e2 $e3 stmtList)
//	| stmtList             -> ^(FOR ^(LOCAL ID) $e1 $e2 stmtList)
//	)
//	END
//	;

forStmt
	:
	FOR ID '=' sequence stmtList END -> ^(FOR ^(LOCAL ID) sequence stmtList)
	;

ifStmt	
	:
	IF expr s1=stmtList 
	( ELSE s2=stmtList -> ^(IF expr $s1 $s2)
	|                  -> ^(IF expr $s1)
	)
	END	
	;


argList
	: '(' (expr (',' expr)*)? ')' -> ^(ARGS expr*)
	;
	
rvalue
	: (varref | '[' varref (','? varref)* ']') -> varref+
	;
	
varref
scope {
    String sym;
} 	
//	: v=ID {$varref::sym=addLocal($v.text).toString();} ('(' expr ')')? -> ^(VAR[v,$varref::sym] $v expr?)
	: v=ID {$varref::sym=addLocal($v.text).toString();} ('(' expr ')')? 
	 ( {isExtern($varref::sym)}? -> ^(EXTERN $v expr?)
	 | {isGlobal($varref::sym)}? -> ^(GLOBAL $v expr?)
	 | {isLocal($varref::sym)}?  -> ^(LOCAL $v expr?)
	 )
	;

expr
	: orExpr
	;

orExpr	:	andExpr ('||'^ andExpr)*
	;

andExpr	:	equalityExpr ('&&'^ equalityExpr)*
	;
	
equalityExpr
	:	comparisonExpr (('=='|'~=')^ comparisonExpr)*
	;

comparisonExpr
	:	additiveExpr (('>'|'<'|'<='|'>=')^ additiveExpr)*
	;

additiveExpr
	:	multiplicativeExpr (('+'|'-')^ multiplicativeExpr)*
	;

multiplicativeExpr
	:	powerExpr (('*'|'.*'|'/'|'./'|'\\')^ powerExpr)*
	;

powerExpr
	:	negationExpr (('^'|'.^')^ powerExpr)?
	;
	
notExpr
	:	(op='~')? negationExpr 	-> {$op != null}? ^(NOT negationExpr)
					-> negationExpr
	;

negationExpr
	:	(op='-')? atom 	-> {$op != null}? ^(NEGATE atom)
				-> atom
	;

atom	
options {k=3;}
	:	'(' expr ')' ->  expr
		| {isFunction(nextToken())}? fncall
		| varref
		| num 
		| vector
		| sequence
	;
        catch [NoViableAltException fpe] {
            String tvt = nextToken();
            String msg = "Undefined Identifier: "+tvt;
    	    throw new RuntimeException(msg);
    	}

fncall
	:  {isFunction(nextToken())}? ID argList -> ^(CALL ID argList)
	;
        catch [FailedPredicateException fpe] {
            String tvt = nextToken();
            String msg = "Undefined Function: "+tvt;
    	    throw new RuntimeException(msg);
    	}

num	: (n=INT|n=FLOAT) -> ^(NUM $n)
	;

sequence 
	: '[' e1=expr ':' e2=expr 
	( ':' e3=expr ']' -> ^(SEQUENCE $e1 $e2 $e3)
	| ']'             -> ^(SEQUENCE $e1 $e2)
	)
	;

vector
	: '[' vectElement ']' -> ^(VECTOR vectElement)
	;

vectElement
	: (varref|num)*
	;

TEMPLATE:	'template'
	;
USING	:	'using'
	;
EXTERN	:	'extern'
	;
GLOBAL	:	'global'
	;
FUNCTION:	'function'
	;
IF	:	'if'
	;
ELSE	:	'else'
	;
END	:	'end'
	;
WHILE	:	'while'
	;
FOR	:	'for'
	;
RETURN	:	'return'
	;
BREAK	:	'break'
	;
CONTINUE:	'continue'
	;

ID	:	CHAR(LCHAR|DIGIT)*	
;

fragment LCHAR
	:	CHAR|'_'
;

fragment CHAR
	:	LC|UC
;

fragment LC
	:	'a'..'z'
	;

fragment UC
	:	'A'..'Z'
	;


STRING_LITERAL
    :  '"' ( EscapeSequence | ~('\\'|'"') )* '"' {setText(getText().substring(1, getText().length()-1));}
    ;

fragment
EscapeSequence
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    ;

INT	:	'0'|('1'..'9' DIGIT*)
	;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
	;

fragment 
EXPONENT 
	:	('E'|'e')('+'|'-')? DIGIT+
	;

fragment 
DIGIT
	:	'0'..'9'
;

WS	:	('\t'|' '|NEWLINE)+ {$channel=HIDDEN;}
;

fragment 
NEWLINE
	:	'\r'|'\n'
;

COMMENT	:	'%{' (options {greedy=false;}:.)* '%}' {$channel=HIDDEN;}
;
		
LINECOMMENT
	:	'%' ~(NEWLINE)* NEWLINE {$channel=HIDDEN;}
;

EOS
	:	';'
	;


