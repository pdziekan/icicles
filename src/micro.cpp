#include <iostream>

#include "micro.hpp"

namespace ll = libcloudphxx::lgrngn;

template <typename real_t>
void micro<real_t>::init(std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls, const micro<real_t>::params_t &params)
{
  for (auto backend : std::set<ll::backend_t>({ll::CUDA, ll::OpenMP, ll::serial}))
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
}

template <typename real_t>
void micro<real_t>::step(std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls)//, const micro_params_t<real_t> &params);
{

}

template struct micro<float>;
template struct micro<double>;
