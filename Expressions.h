class BinValEvalFactory;
class CommonEvaluator;

CommonEvaluator*	CreateBinaryOp(const BinValEvalFactory& operation, CommonEvaluator *a, CommonEvaluator *b, bool allowMix);

CommonEvaluator*	CreateNegateOp(CommonEvaluator *a);
