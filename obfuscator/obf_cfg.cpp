#include "obf_cfg.h" 

StatementMatcher stmtCallMatcher = callExpr().bind("stmt");

void StmtCallback::run(const MatchFinder::MatchResult &result) {
    srcMgr = result.SourceManager;
   /* 
    SourceRange srcRange = funcDecl->getNameInfo().getSourceRange();
    CharSourceRange charSrcRange = CharSourceRange::getTokenRange(srcRange);
    Replacement rep(*srcMgr, charSrcRange, strNewName, LangOptions());
    replace->insert(rep);
    outs() << "Replace FunctionDecl: " << funcName << " to " << strNewName << "\n";
*/
}
