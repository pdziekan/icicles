#include <iostream>
#include <list>
#include <future>
#include "micro.hpp"

namespace ll = libcloudphxx::lgrngn;

template <typename real_t>
void micro<real_t>::init(
  std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls,
  const params_t &params
)
{
  for (auto backend : std::list<ll::backend_t>({ll::CUDA, ll::OpenMP, ll::serial}))
  {
    try
    {
      prtcls.reset(ll::factory<real_t>(
        backend, 
        params.cloudph_opts_init
      ));
      break;
    } 
    catch (const std::exception &e)
    {
      std::cerr  << e.what() << std::endl;
    }
  }
  // TODO: prtcls->init()... (currently in main.hpp)
}

template <typename real_t>
void micro<real_t>::step(
  const params_t &params,
  std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls,
  ll::arrinfo_t<real_t> th,
  ll::arrinfo_t<real_t> rv,
  ll::arrinfo_t<real_t> rhod,
  ll::arrinfo_t<real_t> Cx,
  ll::arrinfo_t<real_t> Cy,
  ll::arrinfo_t<real_t> Cz
)
{
  if (ftr.valid()) // will be invalid in the first timestep
  {
    ftr.wait(); // wating for the async from previous timesteps to finish
  }

  // running synchronous stuff
  // TODO: Cx, Cy and Cz are multiplied by rhod, should we devide them here?
  prtcls->step_sync(params.cloudph_opts, th, rv, Cx, Cy, Cz, rhod); 

  ftr = std::async(
    std::launch::async, 
    &ll::particles_t<real_t, ll::CUDA>::step_async, 
    dynamic_cast<ll::particles_t<real_t, ll::CUDA>*>(prtcls.get()),
    params.cloudph_opts
  );
}

template<typename real_t>
std::future<real_t> micro<real_t>::ftr = std::future<real_t>();

template struct micro<float>;
template struct micro<double>;
