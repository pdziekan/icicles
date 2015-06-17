#include "opts.hpp"

template <typename real_t>
void opts(typename micro<real_t>::params_t &p)
{
  p.cloudph_opts_init.dt = 1;
  p.cloudph_opts_init.sd_conc_mean = 64;

  p.cloudph_opts.coal = false;
}

template void opts<double>(micro<double>::params_t &p);
