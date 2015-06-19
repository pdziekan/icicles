#pragma once
#include <libmpdata++/solvers/mpdata_rhs_vip_prs.hpp>
#include <libmpdata++/concurr/cxx11_thread.hpp>
#include <libmpdata++/output/hdf5_xdmf.hpp>

//#include <libcloudph++/lgrngn/particles.hpp>

#include "micro.hpp"
#include "opts.hpp"

namespace lm = libmpdataxx;
namespace ll = libcloudphxx::lgrngn;

  // compile-time parameters
struct ct_params_t : lm::ct_params_default_t
{
  using real_t = double;
  enum { n_dims = 3 };
  enum { n_eqns = 5 };
  enum { opts = lm::opts::nug };
  enum { rhs_scheme = lm::solvers::trapez };
  enum { prs_scheme = lm::solvers::cr };
  struct ix { enum {
    u, v, w, th, rv, 
    vip_i=u, vip_j=v, vip_k=w, vip_den=-1
  }; };
};  

struct slv_t : lm::output::hdf5_xdmf<lm::solvers::mpdata_rhs_vip_prs<ct_params_t> >
{
  using real_t = typename ct_params_t::real_t;
  using parent_t =  lm::output::hdf5_xdmf<lm::solvers::mpdata_rhs_vip_prs<ct_params_t> >;

  std::unique_ptr<ll::particles_proto_t<real_t>> prtcls;

  void diag()
  {
    prtcls->diag_sd_conc();
    this->record_aux("sd_conc", prtcls->outbuf());
  }

  
  ll::arrinfo_t<real_t> make_arrinfo(
    typename parent_t::arr_t arr
  ) {
    return ll::arrinfo_t<real_t>(
      arr.dataZero(), 
      arr.stride().data()
    );
  }

  void hook_ante_loop(int nt)
  {
    parent_t::hook_ante_loop(nt); 

    if (this->rank == 0) 
    {
      prtcls->init(
        make_arrinfo(this->mem->advectee(ct_params_t::ix::th)),
        make_arrinfo(this->mem->advectee(ct_params_t::ix::rv)),
        make_arrinfo(this->mem->g_factor())
      );
      diag();
    }
  }

  void hook_post_step()
  {
    parent_t::hook_post_step(); // includes output
    if (this->rank == 0)
    {
      micro<real_t>::step(prtcls);
     // performing diagnostics
      if (this->timestep % this->outfreq == 0) diag();
    }
  }

  struct rt_params_t : parent_t::rt_params_t 
  { 
    int nt;
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

