import sklearn.datasets as skds
import numpy as np

digits = skds.load_digits()
arr = np.arange(10)
print digits.data.shape[0], digits.data.shape[1], len(arr)
for i in xrange(digits.data.shape[0]):
  print ' '.join(map(str, digits.data[i,:]))
  print ' '.join(map(str, np.int8(arr == digits.target[i])))
