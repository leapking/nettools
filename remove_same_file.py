# coding:utf-8
import os
import sys
import filecmp
import hashlib

def get_file_md5(file_name):
	"""
	计算文件的md5
	:param file_name:
	:return:
	"""
	m = hashlib.md5()   #创建md5对象
	with open(file_name,'rb') as fobj:
		while True:
			data = fobj.read(4096)
			if not data:
				break
			m.update(data)  #更新md5对象

	return m.hexdigest()	#返回md5对象

def get_str_md5(content):
	"""
	计算字符串md5
	:param content:
	:return:
	"""
	m = hashlib.md5(content) #创建md5对象
	return m.hexdigest()

# 将指定目录下的所有文件的路径存储到all_files变量中
def get_all_files(path):
	all_files = []
	files = os.listdir(path)
	for f in files:
		all_files.append(os.path.join(path, f))
	return all_files

def remove_same_file2(all_files):
	# 用双重for循环来比较文件是否有重复
	for x in all_files:
		for y in all_files:
			# 如果x和y不是相同的文件，而且都存在，则执行后续操作
			if x != y and os.path.exists(x) and os.path.exists(y):
				# 比较两个文件的内容是否一致
				if filecmp.cmp(x, y):
					# 如果一致，则删除第二个，保留第一个，并输出信息
					#os.remove(y)
					print(y + "is same as " + x)

def remove_same_file2(all_files):
	dict_files = {}
	for file in all_files:
		fmd5 = get_file_md5(file)
		if dict_files.get(fmd5) is None:
			dict_files[fmd5] = file
		else:
			print("%s is same as %s\n" % (file, dict_files[fmd5]))
			os.remove(file)

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print("ERROR: %s need a path" % (sys.argv[0]))
		print("Usage: %s path" % (sys.argv[0]))
		exit(1)

	path = sys.argv[1]
	all_files = get_all_files(path) # 调用函数，获取文件列表
	remove_same_file2(all_files)
