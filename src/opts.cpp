#include "opts.hpp"
#include <libcloudph++/common/lognormal.hpp>
#include <libcloudph++/common/unary_function.hpp>
#include <boost/assign/ptr_map_inserter.hpp>


namespace ll = libcloudphxx::lgrngn;
namespace lognormal = libcloudphxx::common::lognormal;


// lognormal aerosol distribution
template <typename real_t>
struct log_dry_radii : public libcloudphxx::common::unary_function<real_t>
{
  const quantity<si::length, real_t>
    mean_rd1 = real_t(.04e-6 / 2) * si::metres,
    mean_rd2 = real_t(.15e-6 / 2) * si::metres;
  const quantity<si::dimensionless, real_t>
    sdev_rd1 = real_t(1.4),
    sdev_rd2 = real_t(1.6);
  const quantity<power_typeof_helper<si::length, static_rational<-3>>::type, real_t>
    n1_stp = real_t(60e6) / si::cubic_metres,
    n2_stp = real_t(40e6) / si::cubic_metres;

  real_t funval(const real_t lnrd) const
  {
    return real_t((
        lognormal::n_e(mean_rd1, sdev_rd1, n1_stp, quantity<si::dimensionless, real_t>(lnrd)) +
        lognormal::n_e(mean_rd2, sdev_rd2, n2_stp, quantity<si::dimensionless, real_t>(lnrd)) 
      ) * si::cubic_metres
    );
  }

  log_dry_radii *do_clone() const 
  { return new log_dry_radii( *this ); }
};

template <typename real_t>
void opts(typename micro<real_t>::params_t &p)
{
  // opts_init
  p.cloudph_opts_init.dt = 1;
  p.cloudph_opts_init.sd_conc_mean = 64;

  // domain size
  p.cloudph_opts_init.nx = 10;
  p.cloudph_opts_init.ny = 10;
  p.cloudph_opts_init.nz = 10;
  p.cloudph_opts_init.dx = 1.;
  p.cloudph_opts_init.dy = 1.;
  p.cloudph_opts_init.dz = 1.;

  boost::assign::ptr_map_insert<
    log_dry_radii<real_t> // value type
  >(
    p.cloudph_opts_init.dry_distros // map
  )(
    0.1 // key TODO: its kappa
  );

  // coalescence options
  p.cloudph_opts_init.kernel = ll::kernel_t::geometric;
  p.cloudph_opts_init.sstp_coal = 1;

  // opts
  p.cloudph_opts.coal = false;
  p.cloudph_opts.sedi = false;
  p.cloudph_opts.cond = false;
  p.cloudph_opts.adve = true;
}

template void opts<double>(micro<double>::params_t &p);
