## Copyright (c) 2011, Pacific Biosciences of California, Inc.
##
## All rights reserved.
##
## THIS SOFTWARE CONSTITUTES AND EMBODIES PACIFIC BIOSCIENCES' CONFIDENTIAL
## AND PROPRIETARY INFORMATION.
##
## Disclosure, redistribution and use of this software is subject to the
## terms and conditions of the applicable written agreement(s) between you
## and Pacific Biosciences, where "you" refers to you or your company or
## organization, as applicable.  Any other disclosure, redistribution or
## use is prohibited.
##
## THIS SOFTWARE IS PROVIDED BY PACIFIC BIOSCIENCES AND ITS CONTRIBUTORS "AS
## IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
## THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
## OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
## WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
## OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
## ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Author: David Alexander

## This is a convenience script for installing Quiver.  By default,
## the script will set up a new virtualenv called VE-QUIVER located in
## $HOME/VE-QUIVER.  Post-installation, the user can activate
## activates this virtualenv by calling
##
##     $ source ~/VE-QUIVER/bin/activate
##
## System requirements:
##   - SWIG  >= 2.0.7
##   - boost >= 1.47
##   - Python 2.7
##   - Python virtualenv
##
## Running:
##   You can download and run directly via:
##
##     $ curl https://raw.github.com/PacificBiosciences/ConsensusCore/master/tools/install-quiver.sh | bash
##
## Uninstall:
##
##     $ rm -rf ~/VE-QUIVER
##

function die(){
 echo "$1; aborting."
 exit 1
}

echo "* Checking basic requirements"
git --version         || die "git not found"
python2.7 --version   || die "Python2.7 not found"
virtualenv --version  || die "Python virtualenv not found"

echo "* Building virtualenv"
cd

# If the virtualenv already exists, we clobber it and start afresh.
test -e VE-QUIVER && rm -rf VE-QUIVER
virtualenv -q -p python2.7 VE-QUIVER || die "Failed to build virtualenv"
source ~/VE-QUIVER/bin/activate

echo "* Installing NumPy and h5py (may take a minute)"
pip install -q numpy==1.6.0 || die "Failed to install NumPy"
pip install -q h5py==2.0.1  || die "Failed to install h5py"

echo "* Installing pbcore"
pip install -q git+https://github.com/PacificBiosciences/pbcore || die "Failed to install pbcore"

echo "* Installing ConsensusCore"
TMPDIR=`mktemp -d /tmp/QuiverInstall-XXXXX`
pushd $TMPDIR
git clone https://github.com/PacificBiosciences/ConsensusCore || die "Failed to download ConsensusCore"
pushd ConsensusCore
python setup.py -q install || die "Failed to install ConsensusCore"
popd
popd

echo "* Installing GenomicConsensus"
pip install -q git+https://github.com/PacificBiosciences/GenomicConsensus || die "Failed to install GenomicConsensus"

echo
echo "*****************************************************"
echo "Successful installation!  To use the variantCaller.py"
echo "script (quiver driver), first enable the virtualenv  "
echo "by running                                           "
echo
echo "    $ source ~/VE-QUIVER/bin/activate                "
echo
echo "*****************************************************"
echo
