#include <libmpdata++/solvers/mpdata_rhs_vip_prs.hpp>
#include <libmpdata++/concurr/cxx11_thread.hpp>
#include <libmpdata++/output/hdf5_xdmf.hpp>

#include "micro.hpp"

// 
using namespace libmpdataxx;
//namespace ll = libcloudphxx::lgrngn;

template<typename ct_params_t>
struct slv_t : solvers::mpdata_rhs_vip_prs<ct_params_t>
{
  using real_t = typename ct_params_t::real_t;
  using parent_t =  solvers::mpdata_rhs_vip_prs<ct_params_t>;

  std::unique_ptr<ll::particles_proto_t<real_t>> prtcls;

  void hook_post_step()
  {
    parent_t::hook_post_step(); // includes output
    if (this->rank == 0) micro<real_t>::step(prtcls);
  }

  struct rt_params_t : parent_t::rt_params_t 
  { 
    struct micro<real_t>::params_t micro_params;
  };

  slv_t( 
    typename parent_t::ctor_args_t args, 
    const rt_params_t &params
  ) : 
    parent_t(args, params)
  {
    if (this->rank == 0) micro<real_t>::init(prtcls, params.micro_params);
  }  
};




int main()
{
  // compile-time parameters
  struct ct_params_t : ct_params_default_t
  {
    using real_t = double;
    enum { n_dims = 3 };
    enum { n_eqns = 5 };
    enum { rhs_scheme = solvers::trapez };
    enum { prs_scheme = solvers::cr };
    struct ix { enum {
      u, v, w, th, rv,
      vip_i=u, vip_j=v, vip_k=w, vip_den=-1
    }; };
  };

  // output choice
  using slv_out_t = output::hdf5_xdmf<slv_t<ct_params_t> >;
  
  // concurency choice
  using run_t = concurr::cxx11_thread<
    slv_out_t,
    bcond::cyclic, bcond::cyclic,
    bcond::cyclic, bcond::cyclic,
    bcond::rigid, bcond::rigid
  >;      

  // run-time parameters
  typename slv_out_t::rt_params_t p;

  int nt = 100;
  ct_params_t::real_t dx = 0.1;

  p.grid_size = { 10, 10, 10 };
  p.outfreq = 20; 
  using ix = typename ct_params_t::ix;
  p.outvars = {
    {ix::u,   {.name = "u",   .unit = "m/s"}}, 
    {ix::v,   {.name = "v",   .unit = "m/s"}}, 
    {ix::w,   {.name = "w",   .unit = "m/s"}}, 
    {ix::th,   {.name = "th",   .unit = "K"}}, 
    {ix::rv,   {.name = "rv",   .unit = "ratio?"}}, 
  };
  p.outdir = "/home/pracownicy/pdziekan/praca/code/icicles/wyniki/";
  p.prs_tol = 1e-7;
 
  // instantiation
  run_t run(p);

  // initial condition
  using ix = typename ct_params_t::ix;
  run.advectee(ix::u) = 0.;
  run.advectee(ix::v) = 0.;
  run.advectee(ix::w) = 0.;
  run.advectee(ix::th) = 0.;
  run.advectee(ix::rv) = 0.;

  // integration
  run.advance(nt);
}
