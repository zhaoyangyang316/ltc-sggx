import numpy as np
import OpenEXR
import Imath

def save_npy_as_exr(npy_file, exr_file):
    data = np.load(npy_file)
    height, width = data.shape[:2]
    header = OpenEXR.Header(width, height)

    channels = ['R','G','B']
    data_dict = {
        'R': data[:, :, 0].tobytes(),
        'G': data[:, :, 1].tobytes(),
        'B': data[:, :, 2].tobytes(),
    }

    for channel in channels:
        header['channels'][channel] = Imath.Channel(Imath.PixelType(Imath.PixelType.FLOAT))
    
    exr = OpenEXR.OutputFile(exr_file, header)
    exr.writePixels(data_dict)
    exr.close()

###save_npy_as_exr('ltc_table_single_sggx.npy', 'ltc_table_single_sggx.exr')
save_npy_as_exr('ltc_table_sheen_approx.npy', 'ltc_table_single_sggx.exr')

