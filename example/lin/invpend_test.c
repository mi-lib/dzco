/* this is a test code of optimal regulator and observer.
 * inverted pendulum figured as below is controlled.
 *
 *             O m
 * -g |     |q/
 *    V     |/ L
 *        __/__
 *       [__M__] -> u
 *        o   o
 *-------------------------
 *          x
 *
 * The strict equation of motion is:
 * 
 *|~d^2x/dt^2~| ________1_____|~ L     -mLcos q~||~mL(dq/dt)^2sin q + u~|
 *|_d^2q/dt^2_|=(M+m sin^2 q)L|_-cos q    M+m  _||_         g sin q    _|
 *
 * And, the linearized equation of motion is:
 *
 * d^2x/dt^2 =    -mg/M q + 1/M u
 * d^2q/dt^2 = (M+m)g/ML q - 1/ML u
 *
 * The former equation is used for simulation, while the latter stands
 * for the observer.
 *
 * The controller designed as the optimal regulator regulates the
 * inverted pendulum to a standing posture.
 */

#include <dzco/dz_sys.h>

/* mass of the cart */
#define M 3.0
/* mass of the tip */
#define m 1.0
/* length of the inverted pendulum */
#define L 0.6
/* acceleration due to the gravity */
#define G 9.8

/* sampling time */
#define DT 0.002
/* step */
#define STEP 3000

/* dimension of the system */
#define DIM 4


/* approximately linearized nominal model */
dzSys nms;
double input;

void nominal_model_create(void)
{
  dzLin *nm;

  nm = zAlloc( dzLin, 1 );
  dzLinAlloc( nm, DIM );
  /* transition matrix */
  zMatSetElem( nm->a, 0, 2, 1 );
  zMatSetElem( nm->a, 1, 3, 1 );
  zMatSetElem( nm->a, 2, 1, -m*G/M );
  zMatSetElem( nm->a, 3, 1, (M+m)*G/(M*L) );
  /* input coefficient vector */
  zVecSetElem( nm->b, 2, 1.0/M );
  zVecSetElem( nm->b, 3,-1.0/(M*L) );
  /* output coefficient vector */
  zVecSetElem( nm->c, 0, 1 );
  zVecSetElem( nm->c, 1, L );
  /* output transfer coefficient */
  nm->d = 0;
  /* dummy input variable */
  dzSysCreateLin( &nms, nm );
  dzSysInputPtr(&nms,0) = &input;
}

/* regulator & observer gains */
zVec opt_gain, obs_gain;

#define Q1  5.0  /* weight for position regulation */
#define Q2 10.0  /* weight for velocity regulation */
#define R   0.1  /* weight for cart driving force */

void regulator_create(void)
{
  zVec q;
  double r;

  q = zVecCreateList( DIM, Q1, Q1, Q2, Q2 );
  r = R;
  opt_gain = zVecAlloc( DIM );
  if( !dzLinLQR( nms._prm, q, r, opt_gain ) ) exit( 1 );
  zVecFree( q );
}

/* poles of the observer */
#define P1 -10.0
#define P2 -11.0
#define P3 -12.0
#define P4 -13.0

void observer_create(void)
{
  zVec pole;

  pole = zVecCreateList( DIM, P1, P2, P3, P4 );
  obs_gain = zVecAlloc( DIM );
  if( !dzLinCreateObs( nms._prm, pole, obs_gain ) ) exit( 1 );
  zVecFree( pole );
}

void regulator_observer_destroy(void)
{
  zVecFree( opt_gain );
  zVecFree( obs_gain );
}

/* forward dynamics simulation with detailed nonlinear plant model */
double x, theta;
double x_dot, theta_dot;
double x_ddot, theta_ddot;
double xt;

void plant_init(double theta0)
{
  theta = zDeg2Rad( theta0 );
}

void plant_eqm(double in)
{
  double det, c, s;
  double v1, v2;

  c = cos( theta );
  s = sin( theta );
  det = M + m*s*s;
  v1 = m*L*zSqr(theta_dot)*s + in;
  v2 = G*s;

  x_ddot = ( v1 - v2*m*c ) / det;
  theta_ddot = ( -v1*c + v2*(M+m) ) / (det*L);
}

void plant_integrate(void)
{
  x += x_dot * DT;
  x_dot += x_ddot * DT;
  theta += theta_dot * DT;
  theta_dot += theta_ddot * DT;
}

void plant_update(double in)
{
  plant_eqm( in );
  plant_integrate();
}

double plant_output(void)
{
  return ( xt = x + L * sin( theta ) );
}

/* for gp-anim */
void output(int i)
{
  FILE *fp;
  static char buf[BUFSIZ], nbuf[5];

  sprintf( buf, "log/dat%s", itoa_zerofill(i,4,nbuf) );
  fp = fopen( buf, "w" );
  printf( "%f %f %f %f\n", xt, x, dzSysOutputVal(&nms,0), zVecElem(((dzLin *)nms._prm)->x,0) );
  fprintf( fp, "0 0\n" );
  fprintf( fp, "%f 0\n", x );
  fprintf( fp, "%f %f\n", x + L * sin(theta), L * cos(theta) );
  fclose( fp );
}

/* main loop */
#define THETA0 1.0
int main(int argc, char *argv[])
{
  register int i;
  zVec ref;

  nominal_model_create();
  regulator_create();
  observer_create();
  plant_init( argc > 1 ? atof(argv[1]) : THETA0 );
  ref = zVecCreateList( 4, 0.0, 0.0, 0.0, 0.0 );
  if( argc > 2 ) zVecSetElem( ref, 0, atof(argv[2]) );
  for( i=0; i<STEP; i++ ){
    output( i );
    input = dzLinStateFeedback( nms._prm, ref, opt_gain );
    dzSysOutputVal(&nms,0) = dzLinOutput( nms._prm, dzSysInputVal(&nms,0) );
    plant_output();
    dzLinObsUpdate( nms._prm, obs_gain, dzSysInputVal(&nms,0), dzSysOutputVal(&nms,0)-xt, DT );
    plant_update( dzSysInputVal(&nms,0) );
  }
  output( STEP );
  dzSysDestroy( &nms );
  regulator_observer_destroy();
  return 0;
}
