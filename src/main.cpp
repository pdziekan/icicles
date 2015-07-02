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

  opts<typename ct_params_t::real_t>(p.micro_params);

  auto &lcph_opts_init(p.micro_params.cloudph_opts_init);

  lcph_opts_init.x0 = 0;// lcph_opts_init.dx / 2.;
  lcph_opts_init.y0 = 0;//lcph_opts_init.dy / 2.; 
  lcph_opts_init.z0 = 0;//lcph_opts_init.dz / 2.; 
  lcph_opts_init.x1 = (lcph_opts_init.nx ) * lcph_opts_init.dx;
  lcph_opts_init.y1 = (lcph_opts_init.ny ) * lcph_opts_init.dy;
  lcph_opts_init.z1 = (lcph_opts_init.nz ) * lcph_opts_init.dz;
/*  lcph_opts_init.x1 = (lcph_opts_init.nx - .5) * lcph_opts_init.dx;
  lcph_opts_init.y1 = (lcph_opts_init.ny - .5) * lcph_opts_init.dy;
  lcph_opts_init.z1 = (lcph_opts_init.nz - .5) * lcph_opts_init.dz;*/

  p.dt = lcph_opts_init.dt;
  p.di = lcph_opts_init.dx;
  p.dj = lcph_opts_init.dy;
  p.dk = lcph_opts_init.dz;
  p.grid_size = { lcph_opts_init.nx, lcph_opts_init.ny, lcph_opts_init.nz };
  p.outfreq = 1; 
  p.outdir = "/home/pracownicy/pdziekan/praca/code/icicles/wyniki/";
//  p.prs_tol = 1e-7;
  p.nt = 10;


  using ix = typename ct_params_t::ix;
  p.outvars = {
    {ix::u,   {.name = "u",   .unit = "m/s"}}, 
    {ix::v,   {.name = "v",   .unit = "m/s"}}, 
    {ix::w,   {.name = "w",   .unit = "m/s"}}, 
    {ix::th,   {.name = "th",   .unit = "K"}}, 
    {ix::rv,   {.name = "rv",   .unit = "kg water / kg dry air"}}, 
  };
 
  // instantiation
  run_t run(p);

  // initial condition
  using ix = typename ct_params_t::ix;
  run.g_factor() = 1;
  run.advectee(ix::u) = 0.;
  run.advectee(ix::v) = 0.;
  run.advectee(ix::w) = 0.;
  run.advectee(ix::th) = 293.;
//  run.advectee(ix::th)(1, blitz::Range::all(), blitz::Range::all()) = 300.;
  run.advectee(ix::rv) = .01;

  run.advector(0) = .5;
  run.advector(1) = 0;
  run.advector(2) = 0;

  // integration
  run.advance(p.nt);
}
