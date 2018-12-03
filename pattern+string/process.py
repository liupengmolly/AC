#!/usr/bin/env python

def dedup():
	with open("./pattern.txt",'r',encoding='gb18030') as f:
		pattern_set = set()
		for line in f:
			pattern_set.add(line)
		f.close()
	pattern_list = list(pattern_set)
	with open("./new_pattern.txt",'w',encoding="gb18030") as f:
		for line in pattern_list:
			if line.strip()!='': 
				f.write(line)
		f.close()

def get_max_len():
	with open("new_pattern.txt",'r',encoding='gb18030') as f:
		max_len = 0
		max_len_str = ''
		for line in f:
			if len(line)>32:
				print(len(line),line)
			if len(line)>max_len:
				max_len = len(line)
				max_len_str = line
	print("max_len:{},{}".format(max_len_str,max_len))

def stat():
	with open("new_pattern.txt",'r',encoding='gb18030') as f:
		count=0
		for line in f:
			if line == '\n':
				count+=1
		print(count)

if __name__ == '__main__':
	get_max_len()
	# dedup()
	# stat()
			
