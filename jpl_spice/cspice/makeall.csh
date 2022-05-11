echo This script builds the SPICE delivery
echo for the cspice package of the toolkit.
echo
echo The script must be executed from the
echo cspice directory.
echo
cd src
echo
echo Creating cspice
echo
cd cspice
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating csupport
echo
cd csupport
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating brief_c
echo
cd brief_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating chrnos_c
echo
cd chrnos_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating ckbref_c
echo
cd ckbref_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating commnt_c
echo
cd commnt_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating cook_c
echo
cd cook_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating dskbrief_c
echo
cd dskbrief_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating dskexp_c
echo
cd dskexp_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating frmdif_c
echo
cd frmdif_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating inspkt_c
echo
cd inspkt_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating mkdsk_c
echo
cd mkdsk_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating mkspk_c
echo
cd mkspk_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating msopck_c
echo
cd msopck_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating spacit_c
echo
cd spacit_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating spkdif_c
echo
cd spkdif_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating spkmrg_c
echo
cd spkmrg_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating tobin_c
echo
cd tobin_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating toxfr_c
echo
cd toxfr_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
echo
echo Creating versn_c
echo
cd versn_c
chmod u+x mkprodct.csh; ./mkprodct.csh
cd ..
cd ..
echo Toolkit Build Complete
