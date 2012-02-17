// Generated from the script divergence/divergence.m
#include <deque>
#include "kqp.hpp"
#include "probabilities.hpp"
#include "feature_matrix/dense.hpp"


DEFINE_LOGGER(logger, "kqp.test.divergence");

namespace kqp {

// Test: Simple test
int divergence_simpleTest(std::deque<std::string> &args) {
Eigen::MatrixXd mU1(2, 2);
mU1 << 1,0,0,1;
Eigen::MatrixXd mU2(2, 2);
mU2 << 1,0,0,1;
Eigen::VectorXd mS1(2);
mS1 << 0.554700196225229,0.832050294337844;
Eigen::VectorXd mS2(2);
mS2 << 0.934148276858481,0.356885131158937;

double epsilon = 0;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), kqp::AltMatrix<double>::Identity(mU1.cols()), mS1);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), kqp::AltMatrix<double>::Identity(mU2.cols()), mS2);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.617241769730341
// qlogq = -1.46854649056823
double expected_divergence = 0.851304720837892;
KQP_LOG_INFO_F(logger, "Divergence = %g [expected %g]; delta = %g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: Simple test (with epsilon)
int divergence_simpleEpsilonTest(std::deque<std::string> &args) {
Eigen::MatrixXd mU1(2, 2);
mU1 << 1,0,0,1;
Eigen::MatrixXd mU2(2, 2);
mU2 << 1,0,0,1;
Eigen::VectorXd mS1(2);
mS1 << 0.554700196225229,0.832050294337844;
Eigen::VectorXd mS2(2);
mS2 << 0.934148276858481,0.356885131158937;

double epsilon = 0.001;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), kqp::AltMatrix<double>::Identity(mU1.cols()), mS1);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), kqp::AltMatrix<double>::Identity(mU2.cols()), mS2);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.617241769730341
// qlogq = -1.46665540747698
double expected_divergence = 0.849413637746637;
KQP_LOG_INFO_F(logger, "Divergence = %g [expected %g]; delta = %g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: 
int divergence_fullTest(std::deque<std::string> &args) {
Eigen::MatrixXd mU1(10, 4);
mU1 << 0.511776902058545,0.493595402930087,0.0278112035992755,0.373106346436466,0.146161827042146,-0.0939748482417878,-0.741161933016101,0.404485940913994,0.0373146001707346,0.18428251475678,-0.0400890759044607,0.128903904117253,-0.232443878596354,-0.387294814420315,0.115719605229439,0.415621711992351,-0.0773270598690544,-0.304847156527393,0.0074162987848466,0.440245621098325,-0.0116653204573875,-0.291182648472031,0.28851327451061,0.363856348156438,0.35698085579192,-0.0365400957710182,0.572615628846732,0.167109434492563,0.361412910304782,0.014548336914199,0.0394883369256452,0.0557544744570312,-0.189161672737465,0.416058037881296,0.0181121886130587,0.308618331850876,-0.601120845831466,0.459570169154491,0.147771497168153,0.235240527147895;
Eigen::MatrixXd mU2(10, 6);
mU2 << -0.339645789913087,0.633021021565688,0.127354017395648,-0.10376097701631,0.108019179343579,0.335634974807751,0.131248477619636,-0.177597083849448,0.70816823558463,-0.0439828300310084,-0.210878999196698,0.240757637021949,0.146304943361495,-0.574241636838851,-0.148339913690144,0.27418527707082,0.299024570942581,0.277396226679864,-0.291207033668661,-0.0283139184028468,0.114931368662774,0.442888326321149,0.161171505244008,0.372606994515025,-0.617692564435944,-0.392963997551224,-0.036992235660202,-0.402976895021734,0.00818121151886699,0.336070970848553,0.417465878287011,0.224001478111362,0.183513633069532,0.0281867083381784,0.442449604195873,0.403571965151398,0.381681488650421,-0.0424751192443295,-0.24762320651675,-0.597627000085964,0.17004358178303,0.264610441270129,0.117457414337224,-0.00887837970282821,-0.0177485391680709,-0.258758066137893,-0.518760961136217,0.267783444115081,0.20008999111775,0.0407498694120305,-0.200260276501295,0.334999907269176,-0.578441683595153,0.374969000988259,-0.0766356992963708,0.170433705011189,-0.557030191687768,0.128249601228644,0.00434006471777516,0.235494896307691;
Eigen::VectorXd mS1(4);
mS1 << 0.123399530823241,0.185566846752313,0.257295627928975,0.940285308312133;
Eigen::VectorXd mS2(6);
mS2 << 0.0684452538947275,0.11947372340624,0.165055594400295,0.229312244471677,0.283566441682456,0.905982281879074;

double epsilon = 0.001;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), kqp::AltMatrix<double>::Identity(mU1.cols()), mS1);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), kqp::AltMatrix<double>::Identity(mU2.cols()), mS2);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.468338015482813
// qlogq = -1.45841492460938
double expected_divergence = 0.990076909126566;
KQP_LOG_INFO_F(logger, "Divergence = %g [expected %g]; delta = %g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
// Test: 
int divergence_zeroTest(std::deque<std::string> &args) {
Eigen::MatrixXd mU1(10, 4);
mU1 << 0.511776902058545,0.493595402930087,0.0278112035992755,0.373106346436466,0.146161827042146,-0.0939748482417878,-0.741161933016101,0.404485940913994,0.0373146001707346,0.18428251475678,-0.0400890759044607,0.128903904117253,-0.232443878596354,-0.387294814420315,0.115719605229439,0.415621711992351,-0.0773270598690544,-0.304847156527393,0.0074162987848466,0.440245621098325,-0.0116653204573875,-0.291182648472031,0.28851327451061,0.363856348156438,0.35698085579192,-0.0365400957710182,0.572615628846732,0.167109434492563,0.361412910304782,0.014548336914199,0.0394883369256452,0.0557544744570312,-0.189161672737465,0.416058037881296,0.0181121886130587,0.308618331850876,-0.601120845831466,0.459570169154491,0.147771497168153,0.235240527147895;
Eigen::MatrixXd mU2(10, 4);
mU2 << 0.511776902058545,0.493595402930087,0.0278112035992755,0.373106346436466,0.146161827042146,-0.0939748482417878,-0.741161933016101,0.404485940913994,0.0373146001707346,0.18428251475678,-0.0400890759044607,0.128903904117253,-0.232443878596354,-0.387294814420315,0.115719605229439,0.415621711992351,-0.0773270598690544,-0.304847156527393,0.0074162987848466,0.440245621098325,-0.0116653204573875,-0.291182648472031,0.28851327451061,0.363856348156438,0.35698085579192,-0.0365400957710182,0.572615628846732,0.167109434492563,0.361412910304782,0.014548336914199,0.0394883369256452,0.0557544744570312,-0.189161672737465,0.416058037881296,0.0181121886130587,0.308618331850876,-0.601120845831466,0.459570169154491,0.147771497168153,0.235240527147895;
Eigen::VectorXd mS1(4);
mS1 << 0.123399530823241,0.185566846752313,0.257295627928975,0.940285308312133;
Eigen::VectorXd mS2(4);
mS2 << 0.123399530823241,0.185566846752313,0.257295627928975,0.940285308312133;

double epsilon = 0.001;

Density< DenseMatrix<double> > rho(DenseMatrix<double>(mU1), kqp::AltMatrix<double>::Identity(mU1.cols()), mS1);
Density< DenseMatrix<double> > tau(DenseMatrix<double>(mU2), kqp::AltMatrix<double>::Identity(mU2.cols()), mS2);
double divergence = rho.computeDivergence(tau, epsilon);
// plogp = -0.468338015482813
// qlogq = -0.468938669472487
double expected_divergence = 0.000600653989673905;
KQP_LOG_INFO_F(logger, "Divergence = %g [expected %g]; delta = %g", %divergence %expected_divergence %(std::abs(divergence - expected_divergence)));

return std::abs(divergence - expected_divergence) < 1e-10 ? 0 : 1;
}
} // end namespace kqp

