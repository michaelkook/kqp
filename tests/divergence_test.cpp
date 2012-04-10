// Generated from the script divergence/divergence.m
#include <deque>
#include <kqp/kqp.hpp>
#include <kqp/probabilities.hpp>
#include <kqp/feature_matrix/dense.hpp>


DEFINE_LOGGER(logger, "kqp.test.divergence");

namespace kqp {

// Test: Simple test
int divergence_simpleTest(std::deque<std::string> &/*args*/) {
Eigen::MatrixXd mU1(2, 2);
mU1 << 1,0,0,1;
Eigen::MatrixXd mU2(2, 2);
mU2 << 1,0,0,1;
Eigen::VectorXd mS1(2);
mS1 << 0.554700196225229036350867772853,0.832050294337843610037452890538;
Eigen::VectorXd mS2(2);
mS2 << 0.934148276858480786088989589189,0.356885131158936608208875895798;

double epsilon = 0;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU1.cols(),mU1.cols()), mS1, true);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU2.cols(),mU2.cols()), mS2, true);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.617241769730341172106591329793
// qlogq = -1.4685464905682328318192730876
double expected_divergence = 0.85130472083789165971268175781;
KQP_LOG_INFO_F(logger, "Divergence = %.10g [expected %.10g]; delta = %.10g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: Simple test (with epsilon)
int divergence_simpleEpsilonTest(std::deque<std::string> &/*args*/) {
Eigen::MatrixXd mU1(2, 2);
mU1 << 1,0,0,1;
Eigen::MatrixXd mU2(2, 2);
mU2 << 1,0,0,1;
Eigen::VectorXd mS1(2);
mS1 << 0.554700196225229036350867772853,0.832050294337843610037452890538;
Eigen::VectorXd mS2(2);
mS2 << 0.934148276858480786088989589189,0.356885131158936608208875895798;

double epsilon = 0.00100000000000000002081668171172;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU1.cols(),mU1.cols()), mS1, true);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU2.cols(),mU2.cols()), mS2, true);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.617241769730341172106591329793
// qlogq = -1.46665540747697842327568196197
double expected_divergence = 0.849413637746637251169090632175;
KQP_LOG_INFO_F(logger, "Divergence = %.10g [expected %.10g]; delta = %.10g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: 
int divergence_fullTest(std::deque<std::string> &/*args*/) {
Eigen::MatrixXd mU1(10, 4);
mU1 << 0.0653187091314131329511383228237,-0.187925374835322955835437142014,-0.0747552586962289822292149210625,0.360469710604951198984480242871,-0.529509224668491484955268333579,-0.635813355876502450669818244933,-0.148758157713544086897528018198,0.360925117807053053020638344606,0.209976707514191451853236003444,-0.149422874176964343151396974463,0.0824414747389231178376078901238,0.238676769393253246365205200163,0.486867661243494620482152868135,-0.0533428457089248977962547826337,0.442359872454453684209596531218,0.378340721927800383017626018045,-0.117588727577509072341754858826,0.0634960607789257780453340274107,0.313832802358321671931662422139,0.255965064128056085657192397775,0.399818247335081256021993567629,-0.092174376756606488103606977802,-0.445598258140702807583721778428,0.260980369149489743740844005515,0.221217904684050975516740322746,0.0649539716178416104419568455342,-0.515018670787000587552029173821,0.271330572290135352275797231414,-0.398680540429751095743426958506,0.220223749478990482586482357874,0.116724191185890752153753169296,0.33605188417673892420367565137,-0.229039725007889521934956178484,0.636112170190435421979202601506,-0.319718655254152939182432646703,0.269111966862368767738900032782,-0.0140939670754972514593594823396,0.255673030790540989354298062608,0.302764241972272363234708336677,0.383516725571979133491140601109;
Eigen::MatrixXd mU2(10, 7);
mU2 << -0.0960136121992579971706760488814,0.136148679487991863812368364961,0.276964104141754929866436896191,-0.453130218629149150721957539645,-0.36652622489703878816413862296,-0.012722180980943145756079104558,0.25357662880792325443835011356,-0.243286638917476955024099538605,-0.645000549221154906298636433348,-0.0708930838744426805586584805496,0.108270444749808306128535662083,0.0348251630736238215746425339603,0.587895474403922757566931522888,0.218264022023520271043039997494,-0.299371375529934036219259496647,-0.00871792626170954723652251061594,0.473894781477532944791164482012,-0.220864106038216156946418777807,0.378954754055906062326641858817,-0.177838844304164300780257690349,0.283838149452218968527006381919,-0.207323952023541840938847258258,0.190719816312184126116946458751,-0.617114831633089555573690176971,-0.0483585955925133967325812989202,0.520042901150620684447289931995,-0.232171725677969620349472279486,0.309483694117336061157885751527,-0.0401706752399889135962673947233,0.491739801520137520540743025776,-0.327498701313050866179565900893,-0.0308024003776110888441763790979,-0.399188210149032718376815864758,0.41056815835312560070136100876,0.334335448207560004263427799742,-0.196289449018011014080897780332,0.268273860660866336758090255898,0.319285522840913604802892677981,0.40645490797493966095288442375,0.236427458356250258519892781806,0.300632983981909851500802233204,0.332778158027929427120739092061,0.159153504326983830097219652089,-0.448512372010009097156313373489,-0.247797439974845684629300990309,0.0186620033388284548014901531587,-0.204066219539129761750828606637,-0.28217835968113730293893581802,0.398190729354843286902365662172,0.339377181071373990040029866577,0.0537291991527580822807230731541,0.17645883975415888045290557784,0.642019212457362820067885422759,-0.138538920048725477940365635732,-0.280649440675487749086158828504,0.313747559583230140045628786538,-0.260845841280416157470511961947,-0.104975865306682777933211525578,0.0550669462634225739816606903787,-0.138050272889446157265069814457,-0.275907042323468187916546412453,-0.308794230725507434787147076349,0.367382203779710758695387085027,0.741819427622054439197540887108,-0.0117802507676180081425121315419,0.0899209092968409895441084245249,-0.366171832875299807508184812832,0.313344043034434416306055481982,0.236638282351446821438756273892,0.31181868428997150388681802724;
Eigen::VectorXd mS1(4);
mS1 << 0.108458911482891370847703171876,0.281290477877420286301202168033,0.328284236780690408252780798648,0.895176960972945523486998808949;
Eigen::VectorXd mS2(7);
mS2 << 1.24282024328561289463306174319e-08,0.0939115766174087596152020296358,0.109695865565670083729976624909,0.201645359206203522717260057107,0.209661932799659445070261654109,0.256866125842353243413640484505,0.910246257501278299706370944477;

double epsilon = 0.00100000000000000002081668171172;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU1.cols(),mU1.cols()), mS1, true);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU2.cols(),mU2.cols()), mS2, true);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.670536477872711755843226910656
// qlogq = -1.91098806304073010053912184958
double expected_divergence = 1.24045158516801823367359247641;
KQP_LOG_INFO_F(logger, "Divergence = %.10g [expected %.10g]; delta = %.10g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: 
int divergence_zeroTest(std::deque<std::string> &/*args*/) {
Eigen::MatrixXd mU1(10, 4);
mU1 << 0.0653187091314131329511383228237,-0.187925374835322955835437142014,-0.0747552586962289822292149210625,0.360469710604951198984480242871,-0.529509224668491484955268333579,-0.635813355876502450669818244933,-0.148758157713544086897528018198,0.360925117807053053020638344606,0.209976707514191451853236003444,-0.149422874176964343151396974463,0.0824414747389231178376078901238,0.238676769393253246365205200163,0.486867661243494620482152868135,-0.0533428457089248977962547826337,0.442359872454453684209596531218,0.378340721927800383017626018045,-0.117588727577509072341754858826,0.0634960607789257780453340274107,0.313832802358321671931662422139,0.255965064128056085657192397775,0.399818247335081256021993567629,-0.092174376756606488103606977802,-0.445598258140702807583721778428,0.260980369149489743740844005515,0.221217904684050975516740322746,0.0649539716178416104419568455342,-0.515018670787000587552029173821,0.271330572290135352275797231414,-0.398680540429751095743426958506,0.220223749478990482586482357874,0.116724191185890752153753169296,0.33605188417673892420367565137,-0.229039725007889521934956178484,0.636112170190435421979202601506,-0.319718655254152939182432646703,0.269111966862368767738900032782,-0.0140939670754972514593594823396,0.255673030790540989354298062608,0.302764241972272363234708336677,0.383516725571979133491140601109;
Eigen::MatrixXd mU2(10, 4);
mU2 << 0.0653187091314131329511383228237,-0.187925374835322955835437142014,-0.0747552586962289822292149210625,0.360469710604951198984480242871,-0.529509224668491484955268333579,-0.635813355876502450669818244933,-0.148758157713544086897528018198,0.360925117807053053020638344606,0.209976707514191451853236003444,-0.149422874176964343151396974463,0.0824414747389231178376078901238,0.238676769393253246365205200163,0.486867661243494620482152868135,-0.0533428457089248977962547826337,0.442359872454453684209596531218,0.378340721927800383017626018045,-0.117588727577509072341754858826,0.0634960607789257780453340274107,0.313832802358321671931662422139,0.255965064128056085657192397775,0.399818247335081256021993567629,-0.092174376756606488103606977802,-0.445598258140702807583721778428,0.260980369149489743740844005515,0.221217904684050975516740322746,0.0649539716178416104419568455342,-0.515018670787000587552029173821,0.271330572290135352275797231414,-0.398680540429751095743426958506,0.220223749478990482586482357874,0.116724191185890752153753169296,0.33605188417673892420367565137,-0.229039725007889521934956178484,0.636112170190435421979202601506,-0.319718655254152939182432646703,0.269111966862368767738900032782,-0.0140939670754972514593594823396,0.255673030790540989354298062608,0.302764241972272363234708336677,0.383516725571979133491140601109;
Eigen::VectorXd mS1(4);
mS1 << 0.108458911482891370847703171876,0.281290477877420286301202168033,0.328284236780690408252780798648,0.895176960972945523486998808949;
Eigen::VectorXd mS2(4);
mS2 << 0.108458911482891370847703171876,0.281290477877420286301202168033,0.328284236780690408252780798648,0.895176960972945523486998808949;

double epsilon = 0.00100000000000000002081668171172;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU1.cols(),mU1.cols()), mS1, true);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Identity(mU2.cols(),mU2.cols()), mS2, true);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.670536477872711755843226910656
// qlogq = -0.671037319673642107531463807391
double expected_divergence = 0.000500841800930351688236896734452;
KQP_LOG_INFO_F(logger, "Divergence = %.10g [expected %.10g]; delta = %.10g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
} // end namespace kqp

