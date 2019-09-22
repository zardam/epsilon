#include "python_toolbox.h"
#include "../shared/toolbox_helpers.h"
#include <assert.h>
extern "C" {
#include <string.h>
#include <ctype.h>
}

namespace Khicas {

const ToolboxMessageTree forLoopChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange1ArgLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange1ArgLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange2ArgsLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange2ArgsLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange3ArgsLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange3ArgsLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInListLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInListLoop)
};

const ToolboxMessageTree ifStatementChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::IfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfThenStatementWithArg, I18n::Message::Default, false, I18n::Message::IfThenStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfElifElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfElifElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfAndIfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfAndIfElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfOrIfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfOrIfElseStatement)
};

const ToolboxMessageTree whileLoopChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::WhileLoopWithArg, I18n::Message::Default, false, I18n::Message::WhileLoop)
};

const ToolboxMessageTree conditionsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::EqualityConditionWithArg, I18n::Message::Default, false, I18n::Message::EqualityCondition),
  ToolboxMessageTree::Leaf(I18n::Message::NonEqualityConditionWithArg, I18n::Message::Default, false, I18n::Message::NonEqualityCondition),
  ToolboxMessageTree::Leaf(I18n::Message::SuperiorStrictConditionWithArg, I18n::Message::Default, false, I18n::Message::SuperiorStrictCondition),
  ToolboxMessageTree::Leaf(I18n::Message::InferiorStrictConditionWithArg, I18n::Message::Default, false, I18n::Message::InferiorStrictCondition),
  ToolboxMessageTree::Leaf(I18n::Message::SuperiorConditionWithArg, I18n::Message::Default, false, I18n::Message::SuperiorCondition),
  ToolboxMessageTree::Leaf(I18n::Message::InferiorConditionWithArg, I18n::Message::Default, false, I18n::Message::InferiorCondition),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionAnd, I18n::Message::Default, false),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionOr, I18n::Message::Default, false),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionNot, I18n::Message::Default, false)
};


const ToolboxMessageTree MathModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportMath, I18n::Message::KhicasImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromMath, I18n::Message::KhicasImportFromMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandMathFunction, I18n::Message::KhicasMathFunction, false, I18n::Message::KhicasCommandMathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantE, I18n::Message::KhicasConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantPi, I18n::Message::KhicasConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSqrt, I18n::Message::KhicasSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPower, I18n::Message::KhicasPower),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandExp, I18n::Message::KhicasExp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandExpm1, I18n::Message::KhicasExpm1),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog, I18n::Message::KhicasLog),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog2, I18n::Message::KhicasLog2),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog10, I18n::Message::KhicasLog10),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCosh, I18n::Message::KhicasCosh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSinh, I18n::Message::KhicasSinh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTanh, I18n::Message::KhicasTanh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAcosh, I18n::Message::KhicasAcosh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAsinh, I18n::Message::KhicasAsinh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtanh, I18n::Message::KhicasAtanh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCos, I18n::Message::KhicasCos),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSin, I18n::Message::KhicasSin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTan, I18n::Message::KhicasTan),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAcos, I18n::Message::KhicasAcos),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAsin, I18n::Message::KhicasAsin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtan, I18n::Message::KhicasAtan),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtan2, I18n::Message::KhicasAtan2),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCeil, I18n::Message::KhicasCeil),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCopySign, I18n::Message::KhicasCopySign),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFabs, I18n::Message::KhicasFabs),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFloor, I18n::Message::KhicasFloor),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFmod, I18n::Message::KhicasFmod),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFrExp, I18n::Message::KhicasFrExp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLdexp, I18n::Message::KhicasLdexp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandModf, I18n::Message::KhicasModf),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsFinite, I18n::Message::KhicasIsFinite),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsInfinite, I18n::Message::KhicasIsInfinite),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsNaN, I18n::Message::KhicasIsNaN),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTrunc, I18n::Message::KhicasTrunc),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRadians, I18n::Message::KhicasRadians),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDegrees, I18n::Message::KhicasDegrees),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandErf, I18n::Message::KhicasErf),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandErfc, I18n::Message::KhicasErfc),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGamma, I18n::Message::KhicasGamma),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLgamma, I18n::Message::KhicasLgamma)
};

const ToolboxMessageTree KandinskyModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportKandinsky, I18n::Message::KhicasImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromKandinsky, I18n::Message::KhicasImportFromKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandKandinskyFunction, I18n::Message::KhicasKandinskyFunction, false, I18n::Message::KhicasCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGetPixel, I18n::Message::KhicasGetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSetPixel, I18n::Message::KhicasSetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandColor, I18n::Message::KhicasColor),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDrawString, I18n::Message::KhicasDrawString),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFillRect, I18n::Message::KhicasFillRect)
};

const ToolboxMessageTree PlotsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PlotCommandWithArg, I18n::Message::Plot),
  ToolboxMessageTree::Leaf(I18n::Message::PlotSeqCommandWithArg, I18n::Message::PlotSeq),
  ToolboxMessageTree::Leaf(I18n::Message::PlotParamCommandWithArg, I18n::Message::PlotParam),
  ToolboxMessageTree::Leaf(I18n::Message::PlotPolarCommandWithArg, I18n::Message::PlotPolar),
  ToolboxMessageTree::Leaf(I18n::Message::PlotFieldCommandWithArg, I18n::Message::PlotField),
};

const ToolboxMessageTree CASChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ApproxCommandWithArg, I18n::Message::Approx),
  ToolboxMessageTree::Leaf(I18n::Message::CfactorCommandWithArg, I18n::Message::CfactorValue),
  ToolboxMessageTree::Leaf(I18n::Message::CsolveCommandWithArg, I18n::Message::CsolveValue),
  ToolboxMessageTree::Leaf(I18n::Message::DesolveCommandWithArg, I18n::Message::DesolveValue),
  ToolboxMessageTree::Leaf(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, false, I18n::Message::DiffCommand),
  ToolboxMessageTree::Leaf(I18n::Message::ExactCommandWithArg, I18n::Message::Exact),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::FactorValue),
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::IegcdCommandWithArg, I18n::Message::Iegcd),
  ToolboxMessageTree::Leaf(I18n::Message::IfactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::InfinityCommandWithArg, I18n::Message::Infinity),
  ToolboxMessageTree::Leaf(I18n::Message::IntCommandWithArg, I18n::Message::Integral, false, I18n::Message::IntCommand),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::LimitCommandWithArg, I18n::Message::LimitValue),
  ToolboxMessageTree::Leaf(I18n::Message::LinsolveCommandWithArg, I18n::Message::Linsolve),
  ToolboxMessageTree::Leaf(I18n::Message::NormalCommandWithArg, I18n::Message::NormalValue),
  ToolboxMessageTree::Leaf(I18n::Message::PartfracCommandWithArg, I18n::Message::PartfracDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::PowmodCommandWithArg, I18n::Message::Powmod),
  ToolboxMessageTree::Leaf(I18n::Message::ProductCommandWithArg, I18n::Message::Product, false, I18n::Message::ProductCommand),
  ToolboxMessageTree::Leaf(I18n::Message::PtaylCommandWithArg, I18n::Message::PtaylValue),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::RsolveCommandWithArg, I18n::Message::Rsolve),
  ToolboxMessageTree::Leaf(I18n::Message::SimplifyCommandWithArg, I18n::Message::SimplifyValue),
  ToolboxMessageTree::Leaf(I18n::Message::SolveCommandWithArg, I18n::Message::SolveValue),
  ToolboxMessageTree::Leaf(I18n::Message::SumCommandWithArg, I18n::Message::Sum, false, I18n::Message::SumCommand),
};
  
const ToolboxMessageTree RandomModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportRandom, I18n::Message::KhicasImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromRandom, I18n::Message::KhicasImportFromRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandomFunction, I18n::Message::KhicasRandomFunction, false, I18n::Message::KhicasCommandRandomFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGetrandbits, I18n::Message::KhicasGetrandbits),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSeed, I18n::Message::KhicasSeed),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandrange, I18n::Message::KhicasRandrange),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandint, I18n::Message::KhicasRandint),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandChoice, I18n::Message::KhicasChoice),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandom, I18n::Message::KhicasRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandUniform, I18n::Message::KhicasUniform)
};

const ToolboxMessageTree CMathModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportCmath, I18n::Message::KhicasImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromCmath, I18n::Message::KhicasImportFromCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCmathFunction, I18n::Message::KhicasCmathFunction, false, I18n::Message::KhicasCommandCmathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantE, I18n::Message::KhicasConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantPi, I18n::Message::KhicasConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPhase, I18n::Message::KhicasPhase),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPolar, I18n::Message::KhicasPolar),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRect, I18n::Message::KhicasRect),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandExpComplex, I18n::Message::KhicasExp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLogComplex, I18n::Message::KhicasLog),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSqrtComplex, I18n::Message::KhicasSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCosComplex, I18n::Message::KhicasCos),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSinComplex, I18n::Message::KhicasSin)
};

const ToolboxMessageTree TurtleModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportTurtle, I18n::Message::KhicasImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromTurtle, I18n::Message::KhicasImportFromTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTurtleFunction, I18n::Message::KhicasTurtleFunction, false, I18n::Message::KhicasCommandTurtleFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandForward, I18n::Message::KhicasTurtleForward),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBackward, I18n::Message::KhicasTurtleBackward),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandRight, I18n::Message::KhicasTurtleRight),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandLeft, I18n::Message::KhicasTurtleLeft),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGoto, I18n::Message::KhicasTurtleGoto),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandSetheading, I18n::Message::KhicasTurtleSetheading),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandCircle, I18n::Message::KhicasTurtleCircle),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandSpeed, I18n::Message::KhicasTurtleSpeed),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPosition, I18n::Message::KhicasTurtlePosition, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandHeading, I18n::Message::KhicasTurtleHeading, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPendown, I18n::Message::KhicasTurtlePendown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPenup, I18n::Message::KhicasTurtlePenup, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPensize, I18n::Message::KhicasTurtlePensize),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandIsdown, I18n::Message::KhicasTurtleIsdown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandReset, I18n::Message::KhicasTurtleReset, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandShowturtle, I18n::Message::KhicasTurtleShowturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandHideturtle, I18n::Message::KhicasTurtleHideturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandColor, I18n::Message::KhicasTurtleColor, false, I18n::Message::KhicasTurtleCommandColorWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBlue, I18n::Message::KhicasTurtleBlue, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandRed, I18n::Message::KhicasTurtleRed, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGreen, I18n::Message::KhicasTurtleGreen, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandYellow, I18n::Message::KhicasTurtleYellow, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBrown, I18n::Message::KhicasTurtleBrown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBlack, I18n::Message::KhicasTurtleBlack, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandWhite, I18n::Message::KhicasTurtleWhite, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPink, I18n::Message::KhicasTurtlePink, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandOrange, I18n::Message::KhicasTurtleOrange, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPurple, I18n::Message::KhicasTurtlePurple, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGrey, I18n::Message::KhicasTurtleGrey, false)
};

const ToolboxMessageTree modulesChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::MathModule, MathModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::CmathModule, CMathModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::RandomModule, RandomModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::TurtleModule, TurtleModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::KandinskyModule, KandinskyModuleChildren)
};

const ToolboxMessageTree catalogChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPound, I18n::Message::KhicasPound, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPercent, I18n::Message::KhicasPercent, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommand1J, I18n::Message::Khicas1J, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLF, I18n::Message::KhicasLF, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTab, I18n::Message::KhicasTab, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAmpersand, I18n::Message::KhicasAmpersand, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSymbolExp, I18n::Message::KhicasSymbolExp, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandVerticalBar, I18n::Message::KhicasVerticalBar, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSingleQuote, I18n::Message::KhicasSingleQuote, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAbs, I18n::Message::KhicasAbs),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAcos, I18n::Message::KhicasAcos),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAcosh, I18n::Message::KhicasAcosh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAsin, I18n::Message::KhicasAsin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAsinh, I18n::Message::KhicasAsinh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtan, I18n::Message::KhicasAtan),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtan2, I18n::Message::KhicasAtan2),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandAtanh, I18n::Message::KhicasAtanh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBackward, I18n::Message::KhicasTurtleBackward),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandBin, I18n::Message::KhicasBin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBlack, I18n::Message::KhicasTurtleBlack, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBlue, I18n::Message::KhicasTurtleBlue,  false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandBrown, I18n::Message::KhicasTurtleBrown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCeil, I18n::Message::KhicasCeil),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandChoice, I18n::Message::KhicasChoice),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandCircle, I18n::Message::KhicasTurtleCircle),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCmathFunction, I18n::Message::KhicasCmathFunction, false, I18n::Message::KhicasCommandCmathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandColor, I18n::Message::KhicasColor),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandComplex, I18n::Message::KhicasComplex),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCopySign, I18n::Message::KhicasCopySign),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCos, I18n::Message::KhicasCos),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandCosh, I18n::Message::KhicasCosh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDegrees, I18n::Message::KhicasDegrees),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDivMod, I18n::Message::KhicasDivMod),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDrawString, I18n::Message::KhicasDrawString),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantE, I18n::Message::KhicasConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandErf, I18n::Message::KhicasErf),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandErfc, I18n::Message::KhicasErfc),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandEval, I18n::Message::KhicasEval),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandExp, I18n::Message::KhicasExp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandExpm1, I18n::Message::KhicasExpm1),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFabs, I18n::Message::KhicasFabs),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFillRect, I18n::Message::KhicasFillRect),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFloat, I18n::Message::KhicasFloat),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFloor, I18n::Message::KhicasFloor),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandForward, I18n::Message::KhicasTurtleForward),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFmod, I18n::Message::KhicasFmod),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandFrExp, I18n::Message::KhicasFrExp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromCmath, I18n::Message::KhicasImportFromCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromKandinsky, I18n::Message::KhicasImportFromKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromMath, I18n::Message::KhicasImportFromMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromRandom, I18n::Message::KhicasImportFromRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportFromTurtle, I18n::Message::KhicasImportFromTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGamma, I18n::Message::KhicasGamma),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGetPixel, I18n::Message::KhicasGetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandGetrandbits, I18n::Message::KhicasGetrandbits),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGoto, I18n::Message::KhicasTurtleGoto),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGreen, I18n::Message::KhicasTurtleGreen, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandGrey, I18n::Message::KhicasTurtleGrey, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandHeading, I18n::Message::KhicasTurtleHeading, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandHex, I18n::Message::KhicasHex),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandHideturtle, I18n::Message::KhicasTurtleHideturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportCmath, I18n::Message::KhicasImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportKandinsky, I18n::Message::KhicasImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportMath, I18n::Message::KhicasImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportRandom, I18n::Message::KhicasImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImportTurtle, I18n::Message::KhicasImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandInput, I18n::Message::KhicasInput),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandInt, I18n::Message::KhicasInt),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandIsdown, I18n::Message::KhicasTurtleIsdown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsFinite, I18n::Message::KhicasIsFinite),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsInfinite, I18n::Message::KhicasIsInfinite),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandIsNaN, I18n::Message::KhicasIsNaN),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandKandinskyFunction, I18n::Message::KhicasKandinskyFunction, false, I18n::Message::KhicasCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLdexp, I18n::Message::KhicasLdexp),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandLeft, I18n::Message::KhicasTurtleLeft),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLength, I18n::Message::KhicasLength),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLgamma, I18n::Message::KhicasLgamma),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog, I18n::Message::KhicasLog),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog10, I18n::Message::KhicasLog10),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandLog2, I18n::Message::KhicasLog2),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandMathFunction, I18n::Message::KhicasMathFunction, false, I18n::Message::KhicasCommandMathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandMax, I18n::Message::KhicasMax),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandMin, I18n::Message::KhicasMin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandModf, I18n::Message::KhicasModf),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandOct, I18n::Message::KhicasOct),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandOrange, I18n::Message::KhicasTurtleOrange, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPendown, I18n::Message::KhicasTurtlePendown, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPenup, I18n::Message::KhicasTurtlePenup, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPensize, I18n::Message::KhicasTurtlePensize),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPhase, I18n::Message::KhicasPhase),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandConstantPi, I18n::Message::KhicasConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPink, I18n::Message::KhicasTurtlePink, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPolar, I18n::Message::KhicasPolar),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPosition, I18n::Message::KhicasTurtlePosition, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPower, I18n::Message::KhicasPower),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandPrint, I18n::Message::KhicasPrint),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandPurple, I18n::Message::KhicasTurtlePurple, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRadians, I18n::Message::KhicasRadians),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandint, I18n::Message::KhicasRandint),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandom, I18n::Message::KhicasRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandomFunction, I18n::Message::KhicasRandomFunction, false, I18n::Message::KhicasCommandRandomFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRandrange, I18n::Message::KhicasRandrange),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRangeStartStop, I18n::Message::KhicasRangeStartStop),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRangeStop, I18n::Message::KhicasRangeStop),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRect, I18n::Message::KhicasRect),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandRed, I18n::Message::KhicasTurtleRed, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandReset, I18n::Message::KhicasTurtleReset, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandRight, I18n::Message::KhicasTurtleRight),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandRound, I18n::Message::KhicasRound),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandSetheading, I18n::Message::KhicasTurtleSetheading),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSetPixel, I18n::Message::KhicasSetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSeed, I18n::Message::KhicasSeed),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandShowturtle, I18n::Message::KhicasTurtleShowturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSin, I18n::Message::KhicasSin),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSinh, I18n::Message::KhicasSinh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSorted, I18n::Message::KhicasSorted),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandSpeed, I18n::Message::KhicasTurtleSpeed),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSqrt, I18n::Message::KhicasSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandSum, I18n::Message::KhicasSum),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTan, I18n::Message::KhicasTan),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTanh, I18n::Message::KhicasTanh),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTrunc, I18n::Message::KhicasTrunc),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandTurtleFunction, I18n::Message::KhicasTurtleFunction, false, I18n::Message::KhicasCommandTurtleFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandUniform, I18n::Message::KhicasUniform),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandWhite, I18n::Message::KhicasTurtleWhite, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasTurtleCommandYellow, I18n::Message::KhicasTurtleYellow, false),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandImag, I18n::Message::KhicasImag, false, I18n::Message::KhicasCommandImagWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandReal, I18n::Message::KhicasReal, false, I18n::Message::KhicasCommandRealWithoutArg)
};

const ToolboxMessageTree functionsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDefWithArg, I18n::Message::Default, false, I18n::Message::KhicasCommandDef),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandReturn, I18n::Message::Default),
  ToolboxMessageTree::Leaf(I18n::Message::KhicasCommandDebug, I18n::Message::Default)
};

const ToolboxMessageTree matricesChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IdentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::RandmatrixCommandWithArg, I18n::Message::Randmatrix),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::MatpowCommandWithArg, I18n::Message::Matpow),
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::RrefCommandWithArg, I18n::Message::Rref),
  ToolboxMessageTree::Leaf(I18n::Message::KerCommandWithArg, I18n::Message::Ker),
  ToolboxMessageTree::Leaf(I18n::Message::EigenvalueCommandWithArg, I18n::Message::Eigenvalue),
  ToolboxMessageTree::Leaf(I18n::Message::EigenvectorCommandWithArg, I18n::Message::Eigenvector),
};

const ToolboxMessageTree progChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::Functions, functionsChildren),
  ToolboxMessageTree::Node(I18n::Message::ForLoopMenu, forLoopChildren),
  ToolboxMessageTree::Node(I18n::Message::IfStatementMenu, ifStatementChildren),
  ToolboxMessageTree::Node(I18n::Message::WhileLoopMenu, whileLoopChildren),
  ToolboxMessageTree::Node(I18n::Message::ConditionsMenu, conditionsChildren),
};

const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Node(I18n::Message::CAS, CASChildren),
  ToolboxMessageTree::Node(I18n::Message::Prog, progChildren),
  ToolboxMessageTree::Node(I18n::Message::Plots, PlotsChildren),
  ToolboxMessageTree::Node(I18n::Message::Catalog, catalogChildren),
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
  ToolboxMessageTree::Node(I18n::Message::Modules, modulesChildren),
};

const ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);


PythonToolbox::PythonToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
}

bool PythonToolbox::handleEvent(Ion::Events::Event event) {
  if (Toolbox::handleEvent(event)) {
    return true;
  }
  if (event.hasText() && strlen(event.text()) == 1 ) {
    char c = event.text()[0];
    if (UTF8Helper::CodePointIsLetter(c)) {
      scrollToLetter(c);
      return true;
    }
  }
  return false;
}

KDCoordinate PythonToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == Toolbox::LeafCellType && m_messageTreeModel->label() == I18n::Message::IfStatementMenu) {
      /* To get the exact height needed for each cell, we have to compute its
       * text size, which means scan the text char by char to look for '\n'
       * chars. This is very costly and ruins the speed performance when
       * scrolling at the bottom of a long table: to compute a position on the
       * kth row, we call cumulatedHeightFromIndex(k), which calls rowHeight k
       * times.
       * We thus decided to compute the real height only for the ifStatement
       * children of the toolbox, which is the only menu that has special height
       * rows. */
    const ToolboxMessageTree * messageTree = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->children(j));
    return k_font->stringSize(I18n::translate(messageTree->label())).height() + 2*Metric::TableCellLabelTopMargin + (messageTree->text() == I18n::Message::Default ? 0 : Toolbox::rowHeight(j));
  }
  return Toolbox::rowHeight(j);
}

bool PythonToolbox::selectLeaf(int selectedRow) {
  m_selectableTableView.deselectTable();
  ToolboxMessageTree * node = (ToolboxMessageTree *)m_messageTreeModel->children(selectedRow);
  const char * editedText = I18n::translate(node->insertedText());
  // strippedEditedText array needs to be in the same scope as editedText
  char strippedEditedText[k_maxMessageSize];
  if (node->stripInsertedText()) {
    int strippedEditedTextMaxLength = strlen(editedText)+1;
    assert(strippedEditedTextMaxLength <= k_maxMessageSize);
    Shared::ToolboxHelpers::TextToInsertForCommandMessage(node->insertedText(), strippedEditedText, strippedEditedTextMaxLength, true);
    editedText = strippedEditedText;
  }
  sender()->handleEventWithText(editedText, true);
  Container::activeApp()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * PythonToolbox::rootModel() {
  return &toolboxModel;
}

MessageTableCellWithMessage * PythonToolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron* PythonToolbox::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int PythonToolbox::maxNumberOfDisplayedRows() {
 return k_maxNumberOfDisplayedRows;
}

void PythonToolbox::scrollToLetter(char letter) {
  assert(UTF8Helper::CodePointIsLetter(letter));
  /* We look for a child MessageTree that starts with the wanted letter. If we
   * do not find one, we scroll to the first child MessageTree that starts with
   * a letter higher than the wanted letter. */
  char lowerLetter = tolower(letter);
  int index = -1;
  for (int i = 0; i < m_messageTreeModel->numberOfChildren(); i++) {
    char l = tolower(I18n::translate(m_messageTreeModel->children(i)->label())[0]);
    if (l == lowerLetter) {
      index = i;
      break;
    }
    if (index < 0 && l >= lowerLetter && UTF8Helper::CodePointIsLowerCaseLetter(l)) {
      index = i;
    }
  }
  if (index >= 0) {
    scrollToAndSelectChild(index);
  }
}

void PythonToolbox::scrollToAndSelectChild(int i) {
  assert(i >=0 && i<m_messageTreeModel->numberOfChildren());
  m_selectableTableView.deselectTable();
  m_selectableTableView.scrollToCell(0, i);
  m_selectableTableView.selectCellAtLocation(0, i);
}

}

