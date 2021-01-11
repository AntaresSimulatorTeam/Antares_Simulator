import os
import sys

class study:
	"""
		Class study
	"""
	def __init__(self, dir):
		"""
			Constructor
		"""
		self.name = os.path.basename(dir)
		
		# print "+ Building study : %s" % self.name
		self.study_dir = dir
		
		self.files_path = {}
		self.files_path["desktop"] 	= self.study_dir + os.sep + "Desktop.ini"
		self.files_path["general"] 	= self.study_dir + os.sep + "settings" + os.sep + "generaldata.ini"
		self.files_path["study"] 	= self.study_dir + os.sep + "study.antares"
		
	def check_files_existence(self):
		"""
			Checking file existence
		"""
		for nickname, file_path in list(self.files_path.items()):
			if not os.path.isfile(file_path):
				print("	File does not exist : %s" % file_path)
				sys.exit(1)
		
		
	def replace(self, linestart, txt, by, file_nick_name):
		"""
			Replacing text by some other text in file, only in lines starting by linestart
		"""
		# File path
		file = self.files_path[file_nick_name]
				
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Replacing in a tmp content
		for line in content_in:
			if line.startswith(linestart):
				content_out.append(line.replace(txt, by))
			else:
				content_out.append(line)
		
		# Erasing file content with the tmp content (content out)
		with open(file, "w") as f:
			f.writelines(content_out)
		
	def insert(self, previous_line_startswith, what, file_nick_name):
		"""
			Inserting a new line in a file, just after a line starting with previous_line_startswith
			If a line starting with previous_line_startswith already exists, we do nothing
		"""
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Inserting a line in a tmp content
		insert_new_line = False
		for line in content_in:
			if line.startswith(what):
				insert_new_line = False
				content_out.append(line)
				continue
				
			if insert_new_line:
				content_out.append(what + "\n")
				insert_new_line = False
				
			content_out.append(line)
			
			if line.startswith(previous_line_startswith):
				insert_new_line = True
				
		# Erasing file content with the tmp content (content out)
		with open(file, "w") as f:
			f.writelines(content_out)
			
	def set_variable(self, variable, value, file_nick_name):
		"""
			Setting variable with a value in a file
		"""
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Searching variable and setting its value in a tmp content
		for line in content_in:
			if line.strip().startswith(variable):
				content_out.append(variable + " = " + value + "\n")
			else:
				content_out.append(line)
		
		# Erasing file content with the tmp content (content out)
		with open(file, "w") as f:
			f.writelines(content_out)
			
	def set_variable_if_greater_than(self, variable, margin, value_max, file_nick_name):
		"""
			Setting variable with a value in a file
		"""
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Searching variable and setting its value in a tmp content
		for line in content_in:
			if line.strip().startswith(variable):
				value_str = line.strip().split('=')[1].strip()
				# conversion in integer
				if value_str.isdigit():
					if int(value_str) > value_max:
						print('%s  %s : replacing %s with %s' % (margin, variable, value_str, value_max)) 
						content_out.append(variable + " = " + str(value_max) + "\n")
					else:
						print('%s  %s : leaving %s' % (margin, variable, value_str))
						content_out.append(line)						
			else:
				content_out.append(line)
		
		# Erasing file content with the tmp content (content out)
		with open(file, "w") as f:
			f.writelines(content_out)
	
	def rename_variable(self, variable_name, change_into, file_nick_name):
		"""
			In file wearing nick name file_nick_name,
			renaming variable whose name is : variable_name
			into : change_into
		"""
		# File path
		file = self.files_path[file_nick_name]
				
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Replacing in a tmp content
		for line in content_in:
			if variable_name in line:
				content_out.append(line.replace(variable_name, change_into))
			else:
				content_out.append(line)
		
		# Erasing file content with the tmp content (content out)
		with open(file, "w") as f:
			f.writelines(content_out)
			
	def set_variable_if_other_var_equals(self, variable, value_1, if_other_var, value_2, margin, file_nick_name):
		"""
			Sets the variable with a certain value if the other var equals another value
		"""		
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		condition = False
		override_file = False
		
		# Searching the other variable and turn condition to true if it equals value_2
		for line in content_in:
			if line.strip().startswith(if_other_var):
				value_str = line.strip().split('=')[1].strip()
				if value_str == value_2:
					condition = True
					break
		
		if condition:
			for line in content_in:
				if line.strip().startswith(variable):
					current_value = line.strip().split('=')[1].strip()
					if current_value != value_1:
						content_out.append(variable + " = " + value_1 + "\n")
						print('%s  %s : changed from |%s| to |%s|' % (margin, variable, current_value, value_1))
						override_file = True
					else:
						content_out.append(line)
				else:
					content_out.append(line)
		
		# No need to print file if :
		# - condition is not filled
		# - condition is filled but the value of the variable is already correct
		if not override_file:
			return
		
		# Erasing file content with the tmp content (content out)	
		with open(file, "w") as f:
			f.writelines(content_out)
		
		
	def show_variable(self, variable, margin, file_nick_name): 
		"""
			Showing value of variable located in a file
		"""
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Searching variable and setting its value in a tmp content
		for line in content_in:
			if line.strip().startswith(variable):
				print('%s  %s' % (margin, line))
	
	def show_variable_if_equals(self, variable, margin, value, file_nick_name):
		"""
			Showing variable of variable located in a file, only if it equals a certain value
		"""
		# File path
		file = self.files_path[file_nick_name]
		
		# Content to print in file (tmp content)
		content_out = []
		
		# Reading the file content (content in)
		with open(file) as f:
			content_in = f.readlines()
			
		# Searching variable and setting its value in a tmp content
		for line in content_in:
			if line.strip().startswith(variable):
				value_str = line.strip().split('=')[1].strip()
				if value_str == value:
					print('%s  %s' % (margin, line))
		

	def rename(self, new_name):
		"""
			Rename the top dir of the current study
		"""
		new_path = os.path.dirname(self.study_dir) + os.sep + new_name
		os.rename(self.study_dir, new_path)
		