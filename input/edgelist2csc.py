#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar 30 20:10:14 2021

@author: ali
"""

import numpy as np
import pandas as pd
import scipy.sparse as ss
import sys
import subprocess
import os


def edgelist_to_csc(filename, num_vertices):
    data = pd.read_csv(filename, delim_whitespace=True, header=None, dtype=np.uint32)
    rows = data[0]  # Not a copy, just a reference.
    cols = data[1]
    weightsExist = False
    try:
        weights = data[2]
        weightsExist = True
    except KeyError as e:
        # Create weights
        weights = np.random.randint(low=1,high=100, size=len(rows))
        
    matrix = ss.coo_matrix((weights, (rows, cols)), shape=(num_vertices,num_vertices))
    matrix = matrix.tocsc()
    
    out_filename = filename[0:-4] + "_csc"
    out_filename_main = out_filename + "_" + str(num_vertices) + "v_" + str(matrix.indices.size) + "e.txt"
    out_filename_header = out_filename + "_header.txt"
    out_filename_indptr = out_filename + "_indptr.txt"
    out_filename_indices = out_filename + "_indices.txt"
    out_filename_empty = out_filename + "_empty.txt"
    out_filename_weights = out_filename + "_weights.txt"
    
    f = open(out_filename_header, "w")
    f.write(str(num_vertices) + "\n")
    f.write(str(matrix.indices.size) + "\n")
    f.close()


    matrix.indptr.tofile(out_filename_indptr,sep="\n",format="%d")
    matrix.indices.tofile(out_filename_indices,sep="\n",format="%d")
    
    f = open(out_filename_empty, "w")
    f.write("\n")
    f.close()

    syscall_str = "cat " + out_filename_header + " " + out_filename_indptr + " " + out_filename_empty + " " + out_filename_indices +" > " + out_filename_main
    subprocess.check_call(syscall_str, shell=True)
    
    if not weightsExist:
        weights.tofile(out_filename_weights,sep="\n",format="%d")
        subprocess.check_call("paste " + filename + " " + out_filename_weights + " | pr -t > " + filename[0:-4] + "_weighted.txt", shell=True)
    
    weights = matrix.data
    
      
    weights.tofile(out_filename_weights,sep="\n",format="%d")
    subprocess.check_call("cat " + out_filename_main + " " + out_filename_empty + " " + out_filename_weights + " > " + out_filename_main[0:-4] + "_weighted.txt", shell=True)
    
    
    #Delete auxilary files
    os.remove(out_filename_indptr)
    os.remove(out_filename_indices)
    os.remove(out_filename_empty)
    os.remove(out_filename_weights)
    os.remove(out_filename_header)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python edgelist2csr.py <YOUR_INPUT_FILE.txt> <# of vertices>")
        print("**This script make use of UNIX commands such as 'paste', 'cat', 'pr'. It is best used on UNIX-like systems.")
    else:
        print("Input is " + sys.argv[1])
        edgelist_to_csc(sys.argv[1], int(sys.argv[2]))
