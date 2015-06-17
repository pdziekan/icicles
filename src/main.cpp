#include "main.hpp"

int main()
{
  // concurency choice
  using run_t = lm::concurr::cxx11_thread<
    slv_t,
    lm::bcond::cyclic, lm::bcond::cyclic,
    lm::bcond::cyclic, lm::bcond::cyclic,
    lm::bcond::rigid, lm::bcond::rigid
  >;      

  // run-time parameters
  typename slv_t::rt_params_t p;

  p.grid_size = { 10, 10, 10 };
  p.outfreq = 20; 
  p.outdir = "/home/pracownicy/pdziekan/praca/code/icicles/wyniki/";
  p.prs_tol = 1e-7;
  p.nt = 100;

  opts<typename ct_params_t::real_t>(p.micro_params);

  using ix = typename ct_params_t::ix;
  p.outvars = {
    {ix::u,   {.name = "u",   .unit = "m/s"}}, 
    {ix::v,   {.name = "v",   .unit = "m/s"}}, 
    {ix::w,   {.name = "w",   .unit = "m/s"}}, 
    {ix::th,   {.name = "th",   .unit = "K"}}, 
    {ix::rv,   {.name = "rv",   .unit = "ratio?"}}, 
  };
 
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
  run.advance(p.nt);
}
