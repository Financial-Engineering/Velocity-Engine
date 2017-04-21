import jargs.gnu.CmdLineParser;

import org.antlr.runtime.ANTLRInputStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.RecognitionException;
import org.antlr.runtime.tree.CommonTree;
import org.antlr.runtime.tree.CommonTreeNodeStream;
import org.antlr.stringtemplate.PathGroupLoader;
import org.antlr.stringtemplate.StringTemplate;
import org.antlr.stringtemplate.StringTemplateGroup;
import org.antlr.stringtemplate.StringTemplateGroupLoader;
import org.antlr.tool.ErrorManager;

import java.io.*;
import java.util.HashMap;
import java.util.Properties;

public class RazorCompiler {

    private static String baseDir;
    private static String templateCache;
    private static String templateLib;
    private static String intelDir;
    private static String boostDir;

    private static String template;
    private static String outdir;
    private static HashMap<String, Enum> symbols;

    private static String error;

    public static void main(String[] args) throws Exception {

        CmdLineParser parser = new CmdLineParser();

        CmdLineParser.Option debug = parser.addBooleanOption('g', "debug");
        CmdLineParser.Option verbose = parser.addBooleanOption('v', "verbose");
        CmdLineParser.Option output = parser.addStringOption('o', "output");
        CmdLineParser.Option help = parser.addBooleanOption('h', "help");
        CmdLineParser.Option optimize = parser.addBooleanOption('O', "optimize");
        CmdLineParser.Option compile = parser.addBooleanOption('c', "compile");
        CmdLineParser.Option force = parser.addBooleanOption('f', "force");

        String rzScript = null;

        try {
            parser.parse(args);
            String[] otherArgs = parser.getRemainingArgs();
            rzScript = otherArgs[0];
        }
        catch (Exception e) {
            printUsage();
            System.exit(2);
        }

        Boolean debugValue = (Boolean) parser.getOptionValue(debug, Boolean.FALSE);
        Boolean optimizeValue = (Boolean) parser.getOptionValue(optimize, Boolean.FALSE);
        Boolean forceValue = (Boolean) parser.getOptionValue(force, Boolean.FALSE);
        String outFile = (String) parser.getOptionValue(output);
        Boolean compileValue = (Boolean) parser.getOptionValue(compile, Boolean.FALSE);

        template = (outFile == null) ? stripExt(rzScript) : stripExt(outFile);

        compileTemplate(template, compileValue, forceValue, debugValue, optimizeValue);

        System.exit(0);
    }

    private static Boolean isNewer(String file1, String file2) {
        String tmplDir = outdir + "/";
        File scriptFile = new File(tmplDir + file1);
        File dllFile = new File(tmplDir + file2);
        return scriptFile.lastModified() > dllFile.lastModified();
    }

    private static void printUsage() {
        System.err.println("Usage: rzcomp [options] file...");
        System.err.println("Options:");
        System.err.println("  -h,--help             Display this information");
        System.err.println("  -v,--verbose          Display lots of output");
        System.err.println("  -c,--compile          Compile only, do not generate DLL");
        System.err.println("  -f,--force            Force compile");
        System.err.println("  -O,--optimize         Compile optimized for speed");
        System.err.println("  -g,--debug            Compile with debug support");
        System.err.println("  -o,--output <file>    Place output in file");
    }

    private static String stripExt(String str) {
        str = (new File(str)).getName();
        return (str == null || "".equals(str)) ? str : str.substring(0, str.indexOf('.'));
    }

    private static void init(String tmpl) throws NoSuchPropertyException {
        error = "";

        Properties props = new Properties();
        java.net.URL url = ClassLoader.getSystemResource("RazorCompiler.properties");

        try {
            props.load(url.openStream());
            baseDir = props.getProperty("razor.compiler");
            intelDir = props.getProperty("intel.mkl");
            boostDir = props.getProperty("boost");
        } catch (IOException e) {
            error = e.getMessage();
        }

        props = System.getProperties();

        // Use system properties over property file
        baseDir = props.getProperty("razor.compiler", baseDir);
        intelDir = props.getProperty("intel.mkl", intelDir);
        boostDir = props.getProperty("boost", boostDir);

        if (baseDir == null)
            throw new NoSuchPropertyException("property baseDir not found");
        if (intelDir == null)
            throw new NoSuchPropertyException("property intelDir not found");
        if (boostDir == null)
            throw new NoSuchPropertyException("property boostDir not found");

        templateCache = baseDir + "/templateCache";
        templateLib = baseDir + "/templateLib";

        template = tmpl;
        outdir = templateCache + "/" + template;
    }

    public static boolean compileString(String template, String name, String script,
                                         boolean compileOnly, boolean force,
                                         boolean debug, boolean optimize) {
        boolean ret = true;

        try {
            init(template);

            byte[] bArray = script.getBytes();
            CommonTokenStream tokens = tokenizeScript(new ByteArrayInputStream(bArray));
            CommonTreeNodeStream nodes = buildParseTree(tokens);
            StringTemplate code = generateCode(nodes);

            writeCode(name, code.toString());

            ret = true;

            if (!compileOnly)
                buildDLL(name, debug, optimize);
        } catch (IOException e) {
            error = e.getMessage();
        } catch (RecognitionException e) {
            error = e.getMessage();
        } catch (NoSuchPropertyException e) {
            error = e.getMessage();
        } catch (DLLCompilerException e) {
            e.printStackTrace();
        }

        return ret;
    }

    public static boolean compileTemplate(String template,
                                           boolean compileOnly, boolean force,
                                           boolean debug, boolean optimize) {

        boolean ret = false;

        try {
            init(template);

            String scriptFile = outdir + "/" + template + ".rz";

            CommonTokenStream tokens = tokenizeScript(new FileInputStream(scriptFile));
            CommonTreeNodeStream nodes = buildParseTree(tokens);
            StringTemplate code = generateCode(nodes);

            writeCode("", code.toString());

            ret = true;

            if (!compileOnly)
                buildDLL("", debug, optimize);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (RecognitionException e) {
            e.printStackTrace();
        } catch (NoSuchPropertyException e) {
            e.printStackTrace();
        } catch (DLLCompilerException e) {
            e.printStackTrace();
        }

        return ret;
    }

    private static void writeCode(String name, String code) throws IOException {
        String fname = outdir + "/" + template + name + ".cpp";
        BufferedWriter writer = new BufferedWriter(new FileWriter(fname));
        writer.write(code);
        writer.newLine();
        writer.close();
    }

    private static CommonTokenStream tokenizeScript(InputStream script) throws IOException {
        ANTLRInputStream input = new ANTLRInputStream(script);
        RazorLexer lexer = new RazorLexer(input);
        CommonTokenStream tokens = new CommonTokenStream();
        tokens.setTokenSource(lexer);
        return tokens;
    }

    private static CommonTreeNodeStream buildParseTree(CommonTokenStream tokens) throws RecognitionException {
        RazorParser parser = new RazorParser(tokens);
        RazorParser.program_return program = parser.program();
        template = program.name;
        symbols = program.symbols;
        CommonTree tree = (CommonTree) program.getTree();
        return new CommonTreeNodeStream(tree);
    }

    private static StringTemplate generateCode(CommonTreeNodeStream nodes) throws RecognitionException {
        RazorTree walker = new RazorTree(nodes);

        // Replace PathGroupLoader with CommonGroupLoader if jar'ing
        StringTemplateGroupLoader loader = new PathGroupLoader(templateLib + ":" + outdir,
                ErrorManager.getStringTemplateErrorListener());
        StringTemplateGroup.registerGroupLoader(loader);

        StringTemplateGroup templates = StringTemplateGroup.loadGroup(template);

        walker.setTemplateLib(templates);

        RazorTree.program_return program = walker.program(symbols);
        return (StringTemplate) program.getTemplate();
    }

    private static String replSlash(String str) {
        return str.replaceAll("/", "\\\\");
    }

    // Use devenv to compile into DLL
    private static void buildDLL(String name, Boolean optimize, Boolean debug) throws DLLCompilerException {
        StringTemplateGroupLoader loader = new PathGroupLoader(templateLib,
                ErrorManager.getStringTemplateErrorListener());
        StringTemplateGroup.registerGroupLoader(loader);

        StringTemplateGroup templates = StringTemplateGroup.loadGroup("VCProject");

        StringTemplate vcproj = templates.getInstanceOf("project");
        vcproj.setAttribute("name", replSlash(template)+name);
        vcproj.setAttribute("outdir", replSlash(outdir));
        vcproj.setAttribute("boost", replSlash(boostDir));
        vcproj.setAttribute("intel", replSlash(intelDir));

        String proj = vcproj.toString(); // generate project file from template

        String projFile = outdir + "/" + template + name + ".vcproj";

        Process p = null;


        try {
            BufferedWriter buffWriter = new BufferedWriter(new FileWriter(projFile));
            buffWriter.write(proj);
            buffWriter.close();

            ProcessBuilder pb = new ProcessBuilder(baseDir + "/bin/rzmake.cmd", projFile, "Release", template + name);
            pb.redirectErrorStream(false);
            pb.directory(new File(templateCache + "/" + template));

            p = pb.start();

            int exitValue = p.waitFor();

            if (exitValue != 0) {
                StringBuffer error = new StringBuffer();
                BufferedReader stdError = new BufferedReader(new
                        InputStreamReader(p.getErrorStream()));
                for (String line; (line = stdError.readLine()) != null;) {
                    error.append(line);
                }
                throw new DLLCompilerException(error.toString());
            }
        }
        catch (Exception e) {
            throw new DLLCompilerException(e);
        }
        finally {
            if (p!=null)
                p.destroy();
        }
    }

    public static String getError() {
        return error;
    }
}
