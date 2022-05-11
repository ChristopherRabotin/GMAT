#!/usr/bin/env python

from distutils.core import setup
import os,sys

def config_optimize(parent_package='',top_path=None):
    from numpy.distutils.misc_util import Configuration

    config = Configuration('snopt', parent_package, top_path )
    config.set_options(ignore_setup_xxx_py=True,
                       assume_default_configuration=True,
                       delegate_options_to_subpackages=True,
                       quiet=False)

    config.add_data_files('README')
    config.add_subpackage('solvers')
    config.add_subpackage('solvers/py_snopt')
    return config

def setup_optimize():
    from numpy.distutils.core import setup

    setup( name='snopt',
           author='Elizabeth Wong',
           author_email='elwong@ucsd.edu',
           maintainer='UCSD Optimization',
           maintainer_email='elwong@ucsd.edu',
           url="http://ccom.ucsd.edu/~optimizers",
           description='Python interface for optimization solvers',
           long_description='Python interface for optimization solvers',
           platforms=['Windows','Linux','Solaris','Mac OS-X','Unix'],
           configuration=config_optimize )

if __name__ == '__main__':
    setup_optimize()
