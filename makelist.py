import glob

raw_list = glob.glob('/star/institutions/lbl/sunxm/tmp/rexwg/Embed/19GeV/data/*')
outfile = open('./Lambda_test.list', 'w')

for line in raw_list:
    #par1, par2, par3, par4, par5, par6, par7, par8, par9, par10 = line.split('/')
    #a, b = par7.split('.')
    newline = ''.join([line, '\n'])
    outfile.write(newline)

outfile.close()
