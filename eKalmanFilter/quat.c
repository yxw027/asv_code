/*
 * Quaternion and Euler and DCM transformations
 * (c) Srikanth Saripalli 
 * srik@robotics.usc.edu
 * Checked with mathworld and found correct
 */

#include <math.h>
#include "quat.h"

/*
 * This will construct a direction cosine matrix from 
 * euler angles in the standard rotation sequence 
 * [phi][theta][psi] from NED to body frame
 * This ia also called a transform from inertial axis
 * to body axis
 * body = tBL(3,3)*NED
 */
/*
void eulerDC(double tBL[3][3],double phi,double theta,double psi)
{
	const double cpsi	= cos(psi);
	const double cphi	= cos(phi);
	const double ctheta	= cos(theta);
	const double spsi	= sin(psi);
	const double sphi	= sin(phi);
	const double stheta	= sin(theta);

	tBL[0][0] = cpsi*ctheta;
	tBL[0][1] = spsi*ctheta;
	tBL[0][2] = -stheta;

	tBL[1][0] = -spsi*cphi + cpsi*stheta*sphi;
	tBL[1][1] =  cpsi*cphi + spsi*stheta*sphi;
	tBL[1][2] = ctheta*sphi;

	tBL[2][0] =  spsi*sphi + cpsi*stheta*cphi;
	tBL[2][1] = -cpsi*sphi + spsi*stheta*cphi;
	tBL[2][2] = ctheta*cphi;
}
*/

/*
 * This will construct a direction cosine matrix from 
 * quaternions in the standard rotation  sequence
 * [phi][theta][psi] from NED to body frame
 *
 * body = tBL(3,3)*NED
 * q(4,1)
 */

void quatDC(double tBL[3][3],const double q[4])
{
	const double q0 = q[0];
	const double q1 = q[1];
	const double q2 = q[2];
	const double q3 = q[3];

	tBL[0][0] = 1-2*(q2*q2 + q3*q3);
	tBL[0][1] =   2*(q1*q2 + q0*q3);
	tBL[0][2] =   2*(q1*q3 - q0*q2);

	tBL[1][0] =   2*(q1*q2 - q0*q3);
	tBL[1][1] = 1-2*(q1*q1 + q3*q3);
	tBL[1][2] =   2*(q2*q3 + q0*q1);

	tBL[2][0] =   2*(q1*q3 + q0*q2);
	tBL[2][1] =   2*(q2*q3 - q0*q1);
	tBL[2][2] = 1-2*(q1*q1 + q2*q2);
}


/*
 * This will construct the euler omega-cross matrix
 * wx(3,3)
 * p, q, r (rad/sec)
 */

void eulerWx(double Wx[3][3],double p,double q,double r)
{
	Wx[0][0] =  0;
	Wx[0][1] = -r;
	Wx[0][2] =  q;

	Wx[1][0] =  r;
	Wx[1][1] =  0;
	Wx[1][2] = -p;

	Wx[2][0] = -q;
	Wx[2][1] =  p;
	Wx[2][2] =  0;
}

/*
 * This will construct the quaternion omega matrix
 * W(4,4)
 * p, q, r (rad/sec)
 */

void quatW(double W[4][4],double p,double q,double r)
{
	p /= 2.0;
	q /= 2.0;
	r /= 2.0;

	W[0][0] =  0;
	W[0][1] = -p;
	W[0][2] = -q;
	W[0][3] = -r;

	W[1][0] =  p;
	W[1][1] =  0;
	W[1][2] =  r;
	W[1][3] = -q;

	W[2][0] =  q;
	W[2][1] = -r;
	W[2][2] =  0;
	W[2][3] =  p;

	W[3][0] =  r;
	W[3][1] =  q;
	W[3][2] = -p;
	W[3][3] =  0;
}


/*
 * This will normalize a quaternion vector q
 * q/norm(q)
 * q(4,1)
 */

void normq(double q[4])
{
    	int i;
	double	q0 = q[0];
	double	q1 = q[1];
	double	q2 = q[2];
	double	q3 = q[3];

	double	q02 = q0*q0;
	double	q12 = q1*q1;
	double	q22 = q2*q2;
	double	q32 = q3*q3;

	double	invsqr = 1.0 / sqrt(q02 + q12 + q22 + q32);

	for( i=0 ; i<4 ; ++i )
        	q[i] *= invsqr;
}


/*
 * This will convert from quaternions to euler angles
 * q(4,1) -> euler[phi;theta;psi] (rad)
 */

void quat2euler(const double *q,double *euler)
{
	double	q0 = q[0];
	double	q1 = q[1];
	double	q2 = q[2];
	double	q3 = q[3];

	double	theta	= -asin(2*(q1*q3 - q0*q2));
	double	phi	= atan2(2*(q2*q3 + q0*q1),1-2*(q1*q1 + q2*q2));
	double	psi	= atan2(2*(q1*q2 + q0*q3),1-2*(q2*q2 + q3*q3));
    
	euler[0] = phi;
	euler[1] = theta;
	euler[2] = psi;
}


/*
 * This will convert from euler angles to quaternion vector
 * phi, theta, psi -> q(4,1)
 * euler angles in radians
 */

void euler2quat(double q[4],double phi,double theta,double psi)
{
	double	shphi0;
	double	chphi0;

	double	shtheta0;
	double	chtheta0;

	double	shpsi0;
	double	chpsi0;

	phi	/= 2.0;
	theta	/= 2.0;
	psi	/= 2.0;

	shphi0   = sin( phi );
	chphi0   = cos( phi );

	shtheta0 = sin( theta );
	chtheta0 = cos( theta );

	shpsi0   = sin( psi );
	chpsi0   = cos( psi );

	q[0] =  chphi0 * chtheta0 * chpsi0 + shphi0 * shtheta0 * shpsi0;
	q[1] = -chphi0 * shtheta0 * shpsi0 + shphi0 * chtheta0 * chpsi0;
	q[2] =  chphi0 * shtheta0 * chpsi0 + shphi0 * chtheta0 * shpsi0;
	q[3] =  chphi0 * chtheta0 * shpsi0 - shphi0 * shtheta0 * chpsi0;

}
