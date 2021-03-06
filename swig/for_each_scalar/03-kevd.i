
// ---- Rank selection


%include "kqp/rank_selector.hpp"
%shared_ptr(kqp::RankSelector< @STYPE@, true >);
%shared_ptr(kqp::RankSelector< @STYPE@, false >);
%shared_ptr(kqp::RatioSelector< @STYPE@ >);


%template(EigenList@SNAME@) kqp::EigenList< @STYPE@ >;

%ignore kqp::Selector< @STYPE@ >::selection;
shared_template(Selector@SNAME@, kqp::Selector< @STYPE@ >);

shared_template(Aggregator@SNAME@, kqp::Aggregator< @STYPE@ >);
shared_template(AggregatorMax@SNAME@, kqp::Max< @STYPE@ >);
shared_template(AggregatorMean@SNAME@, kqp::Mean< @STYPE@ >);

%ignore kqp::RatioSelector@SNAME@::selection;
%template(RatioSelector@SNAME@) kqp::RatioSelector< @STYPE@ >;

%ignore kqp::RankSelector< @STYPE@, true >::selection;
%template(RankSelectorAbs@SNAME@) kqp::RankSelector< @STYPE@,true >;

%ignore kqp::RankSelector< @STYPE@, false >::selection;
%template(RankSelector@SNAME@) kqp::RankSelector< @STYPE@,false >;

%ignore kqp::ChainSelector< @STYPE@ >::selection;
shared_template(ChainSelector@SNAME@, kqp::ChainSelector< @STYPE@ >);

// --- Decomposition

%include "kqp/decomposition.hpp"
%template(Decomposition@SNAME@) kqp::Decomposition< @STYPE@ >;

// --- Decomposition cleaner

%include "kqp/cleanup.hpp"
%shared_ptr(kqp::Cleaner< @STYPE@ >);
%shared_ptr(kqp::CleanerList< @STYPE@ >);
%shared_ptr(kqp::CleanerRank< @STYPE@ >);

%template(Cleaner@SNAME@) kqp::Cleaner< @STYPE@ >;
%template(CleanerList@SNAME@) kqp::CleanerList< @STYPE@ >;
%template(CleanerRank@SNAME@) kqp::CleanerRank< @STYPE@ >;

%include "kqp/cleaning/qp_approach.hpp"
%shared_ptr(kqp::CleanerQP< @STYPE@ >)
%template(CleanerQP@SNAME@) kqp::CleanerQP< @STYPE@ >;

%include "kqp/cleaning/unused.hpp"
%shared_ptr(kqp::CleanerUnused< @STYPE@ >);
%template(CleanerUnused@SNAME@) kqp::CleanerUnused< @STYPE@ >;

%include "kqp/cleaning/null_space.hpp"
shared_template(CleanerNullSpace@SNAME@, kqp::CleanerNullSpace< @STYPE@ >);



// ---- Kernel EVD

%shared_ptr(kqp::KernelEVD< @STYPE@ >);
%shared_ptr(kqp::DenseDirectBuilder< @STYPE@ >);
%shared_ptr(kqp::AccumulatorKernelEVD< @STYPE@, true >)
%shared_ptr(kqp::AccumulatorKernelEVD< @STYPE@, false >)
%shared_ptr(kqp::DivideAndConquerBuilder< @STYPE@ >)
%shared_ptr(kqp::IncrementalKernelEVD< @STYPE@ >)

%include "kqp/kernel_evd.hpp"
%template(KEVD@SNAME@) kqp::KernelEVD< @STYPE@ >;

%include "kqp/kernel_evd/dense_direct.hpp"
%template(KEVDDirect@SNAME@) kqp::DenseDirectBuilder< @STYPE@ >;

%include "kqp/kernel_evd/accumulator.hpp"
%template(KEVDLCAccumulator@SNAME@) kqp::AccumulatorKernelEVD< @STYPE@, true >;

%include "kqp/kernel_evd/accumulator.hpp"
%template(KEVDAccumulator@SNAME@) kqp::AccumulatorKernelEVD< @STYPE@, false >;

%include "kqp/kernel_evd/incremental.hpp"
%template(KEVDIncremental@SNAME@) kqp::IncrementalKernelEVD< @STYPE@ >;

%include "kqp/kernel_evd/divide_and_conquer.hpp"
%template(KEVDDivideAndConquer@SNAME@) kqp::DivideAndConquerBuilder< @STYPE@ >;
