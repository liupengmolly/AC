#!/usr/bin/env python

def dedup():
	with open("./pattern.txt",'r',encoding='utf-8') as f:
		pattern_set = set()
		for line in f:
			pattern_set.add(line)
		f.close()
	pattern_list = list(pattern_set)
	with open("./new_pattern.txt",'w',encoding="gb18030") as f:
		for line in pattern_list:
			f.write(line)
		f.close()

def get_max_len():
	with open("new_pattern.txt",'r',encoding='gb18030') as f:
		max_len = 0
		for line in f:
			if len(line)>max_len:
				max_len = len(line)
	print("max_len:{}".format(max_len))

if __name__ == '__main__':
	#get_max_len()
	dedup()
			
