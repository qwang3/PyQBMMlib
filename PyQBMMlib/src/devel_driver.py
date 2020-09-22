from qbmm_manager import *
import sys
sys.path.append('../utils/')
from stats_util import *
from pretty_print_util import *

def transport_terms_example():

    config = {}
    config['qbmm'] = {}
    config['qbmm']['governing_dynamics'] = ' 0.3*x + 0.6*xdot/(x**4.1)'

    config['qbmm']['num_internal_coords']  = 2
    config['qbmm']['num_quadrature_nodes'] = 2
    config['qbmm']['method']       = 'chyqmom'
    config['qbmm']['adaptive']     = False
    config['qbmm']['max_skewness'] = 30

    qbmm_mgr    = qbmm_manager( config )

    return

def moments_workflow_example():

    config = {}
    config['qbmm'] = {}
    config['qbmm']['governing_dynamics'] = '1+3*x'

    config['qbmm']['num_internal_coords']  = 1
    config['qbmm']['num_quadrature_nodes'] = 2
    config['qbmm']['method']       = 'qmom'
    config['qbmm']['adaptive']     = False

    qbmm_mgr    = qbmm_manager( config )
    num_moments = qbmm_mgr.num_moments
    
    mu    = 1.0
    sigma = 0.1
    moments = raw_gaussian_moments_univar( num_moments, mu, sigma )
    indices = qbmm_mgr.indices

    message = 'devel_driver: main: '
    f_array_pretty_print( message, 'moments', moments )
    i_array_pretty_print( message, 'indices', indices )

    abscissas, weights = qbmm_mgr.moment_invert( moments )
    f_array_pretty_print( message, 'weights', weights )
    f_array_pretty_print( message, 'abscissas', abscissas )
    
    projected_moments = qbmm_mgr.projection( weights, abscissas, indices )
    f_array_pretty_print( message, 'projected_moments', projected_moments )

    recons_error = np.abs( moments - projected_moments ).max()
    f_scalar_pretty_print( message, 'recons_error', recons_error )

    return
    
if __name__ == '__main__':

    np.set_printoptions( formatter = { 'float': '{: 0.4E}'.format } )
    
    transport_terms_example()
    
    ###
    ### [ecg] The following workflow will be encapsulated in a single
    ### qbmm_mgr function called compute_rhs. This function will be
    ### called from advancer, and will take as inputs only the
    ### moments. Here, the steps are laid out explicitly for
    ### development purposes, hence the name of the script
    ### (duh).
    ###
    
    #abscissas, weights = qbmm_mgr.moment_invert( moments )
    #f_array_pretty_print( message, 'weights', weights )
    #f_array_pretty_print( message, 'abscissas', abscissas )
    
    #projected_moments = qbmm_mgr.projection( weights, abscissas, indices )
    #f_array_pretty_print( message, 'projected_moments', projected_moments )

    #recons_error = np.abs( moments - projected_moments ).max()
    #f_scalar_pretty_print( message, 'recons_error', recons_error )

    #if config['qbmm']['num_internal_coords']==1:
    #    # Example coef/exp for xdot = 4x - 2x^2
    #    c0 = symbols('c0')
    #    coef = [ 4*c0, -2*c0 ]
    #    exp  = [   c0,  1+c0 ]

    # Esteban To-do:
    #   Loop below is example of taking Euler time steps
    #   1. I think 'coef' and 'exp' can be moved to the 'config' dictionary
    #       since they are only computed once, maybe they shouldn't be 
    #       passed as functions every time compute_rhs is called?
    #   2. Hyperbolic 1D needs to be able to access max skewness for 3-node closure
    #       max_skewness = config['qbmm']['max_skewness']
    #       wasn't working when i tried it. Look at inversion.py

    ### [ecg]:
    ### 1. coef and exp will be member variables in qbmm_mgr, which will
    ###    determine what they are based on config option governing_dynamics
    ###    using sympy. That way, they will be computed upon construction
    ###    and ready to be used when compute_rhs is called.
    ### 2. Fixed. Note that the routines in inversion.py should not take config
    ###    inputs, and they are only meant to be called from inside qbmm_mgr.
    # dt = 0.1
    # mom = moments
    # for i in range(100):
    #     print('--Time step: ',i, ' --Moments: ',mom)
    #     abscissas, weights = qbmm_mgr.moment_invert( mom )
    #     rhs = qbmm_mgr.compute_rhs(coef,exp,indices,weights,abscissas)
    #     proj = qbmm_mgr.projection( weights, abscissas, indices)
    #     mom = proj + dt*rhs

    exit