#ifdef _OPENMP
  #include <omp.h>
#endif
#include "RcppArmadillo.h"
#include "cpp_extras.h"

// [[Rcpp::depends(RcppArmadillo)]]

using namespace Rcpp;
using namespace arma;
using namespace std;

// [[Rcpp::export]]
arma::mat cpp_cov1_mxPBF_single(arma::mat X, double a0, double b0, double gamma){
  // 1. parameters
  int n = X.n_rows; double nn = static_cast<double>(n);
  int p = X.n_cols;
  
  // 2. prepare
  double term_share = a0*std::log(static_cast<float>(b0)) - Rf_lgammafn(a0) + 0.5*std::log(static_cast<float>(gamma/(1.0+gamma))) + Rf_lgammafn(static_cast<double>(n)/2.0 + a0);
  // 3. iterate
  arma::mat logBFmat(p,p,fill::zeros);
  for (int i=0;i<p;i++){
    for (int j=0;j<p;j++){
      if (i!=j){
        arma::vec Xi = X.col(i);
        arma::vec Xj = X.col(j);
        
        double sXi2 = arma::dot(Xi,Xi);
        double sXj2 = arma::dot(Xj,Xj);
        double sXij = arma::dot(Xi,Xj);
        
        logBFmat(i,j) = term_share + (0.5*sXi2) - (nn/2.0 + a0)*mylog(0.5*(sXi2- (sXij*sXij)/(sXj2*(1+gamma))) + b0);
      }
    }
  }
  // 4.return
  return(logBFmat);
}

// [[Rcpp::export]]
arma::mat cpp_cov1_mxPBF_multiple(arma::mat X, double a0, double b0, double gamma, int nCores){
  // 1. parameters
  int n = X.n_rows; double nn = static_cast<double>(n);
  int p = X.n_cols;
  
  // 2. prepare
  double term_share = a0*mylog(b0) - Rf_lgammafn(a0) + 0.5*mylog(gamma/(1.0+gamma)) + Rf_lgammafn(static_cast<double>(n)/2.0 + a0);
  // 3. iterate
  arma::mat logBFmat(p,p,fill::zeros);
  
  #ifdef _OPENMP
  #pragma omp parallel for num_threads(nCores) collapse(2) shared(X,term_share,nn,a0,b0,gamma)
  for (int i=0;i<p;i++){
    for (int j=0;j<p;j++){
      if (i!=j){
        arma::vec Xi = X.col(i);
        arma::vec Xj = X.col(j);
        
        double sXi2 = arma::dot(Xi,Xi);
        double sXj2 = arma::dot(Xj,Xj);
        double sXij = arma::dot(Xi,Xj);
        
        logBFmat(i,j) = term_share + (0.5*sXi2) - (nn/2.0 + a0)*std::log(static_cast<float>(0.5*(sXi2- (sXij*sXij)/(sXj2*(1+gamma))) + b0));
      }
    }
  }
  #else
  for (int i=0;i<p;i++){
    for (int j=0;j<p;j++){
      if (i!=j){
        arma::vec Xi = X.col(i);
        arma::vec Xj = X.col(j);
        
        double sXi2 = arma::dot(Xi,Xi);
        double sXj2 = arma::dot(Xj,Xj);
        double sXij = arma::dot(Xi,Xj);
        
        logBFmat(i,j) = term_share + (0.5*sXi2) - (nn/2.0 + a0)*mylog(0.5*(sXi2- (sXij*sXij)/(sXj2*(1+gamma))) + b0);
      }
    }
  }
  #endif
  // 4.return
  return(logBFmat);
}