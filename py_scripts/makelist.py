import glob

#raw_list_geant = glob.glob('/eliza17/star/starprod/embedding/AuAu200_production_2011/K0short_100_20143801/P11id.SL11d_embed/2011/*/*geant.root')
#Lambda_Flat_Pt
raw_list_pico_la_flat = glob.glob('/global/homes/l/lwen1990/pwg/embedding/Lambda/output_fp/auau200GeV_run11_la_flat.*.picodst.root')
raw_list_pico_la_exp = glob.glob('/global/homes/l/lwen1990/pwg/embedding/Lambda/output_exp/auau200GeV_run11_la_exp.*.picodst.root')
raw_list_pico_antila_flat = glob.glob('/global/homes/l/lwen1990/pwg/embedding/Lambda/output_fp/auau200GeV_run11_antila_flat.*.picodst.root')
raw_list_pico_antila_exp = glob.glob('/global/homes/l/lwen1990/pwg/embedding/Lambda/output_exp/auau200GeV_run11_antila_exp.*.picodst.root')
#raw_list_event = glob.glob('/eliza17/star/starprod/embedding/AuAu200_production_2011/K0short_100_20143801/P11id.SL11d_embed/2011/*/*event.root')
outfile_la_flat = open('../analysis/picodst_la_flat.list', 'w')
outfile_la_exp = open('../analysis/picodst_la_exp.list', 'w')
outfile_antila_flat = open('../analysis/picodst_antila_flat.list', 'w')
outfile_antila_exp = open('../analysis/picodst_anti_la_exp.list', 'w')

#for line1, line2, line3, line4 in zip(raw_list_pico_la_flat, raw_list_pico_la_exp, raw_list_pico_antila_flat, raw_list_pico_antila_exp):
for line1 in raw_list_pico_la_flat:
    #par1, par2, par3, par4, par5, par6, par7 = line.split('/')
    #a, b = par7.split('.')
    print line1
    newline1 = ''.join([line1, '\n'])
    #newline2 = ''.join([line2, '\n'])
    #newline3 = ''.join([line3, '\n'])
    #newline4 = ''.join([line4, '\n'])
    #newline2 = ''.join([line2, '\n'])
    outfile_la_flat.write(newline1)
    #outfile_la_exp.write(newline2)
    #outfile_antila_flat.write(newline3)
    #outfile_antila_exp.write(newline4)

outfile_la_flat.close()
#outfile_la_exp.close()
#outfile_antila_flat.close()
#outfile_antila_exp.close()
