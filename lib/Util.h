#include "AST.h"

using namespace std;
using namespace clang;
class Util
{
public:
    static string stmtToString(Stmt *st)
    {

        clang::LangOptions lo;
        // PrintingPolicy(lo)
        string out_str;
        llvm::raw_string_ostream outstream(out_str);
        st->printPretty(outstream, NULL, PrintingPolicy(lo));
        return out_str;
    }

    static void printStmt(Stmt *st)
    {

        clang::LangOptions lo;
        // PrintingPolicy(lo)
        string out_str;
        llvm::raw_string_ostream outstream(out_str);
        st->printPretty(outstream, NULL, PrintingPolicy(lo));
        cout << out_str << "\n";
    }
};