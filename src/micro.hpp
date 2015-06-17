#pragma once

#include <libcloudph++/lgrngn/factory.hpp>

namespace ll = libcloudphxx::lgrngn;

template <typename real_t>
struct micro
{
  struct params_t
  {
    int backend = -1;
//    bool async = true;
    ll::opts_t<real_t> cloudph_opts;
    ll::opts_init_t<real_t> cloudph_opts_init;
//    outmom_t<real_t> out_dry, out_wet;
  };

  static void init(std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls, const params_t &params);

  static void step(std::unique_ptr<ll::particles_proto_t<real_t>> &prtcls);//, const micro_params_t<real_t> &params);
};
