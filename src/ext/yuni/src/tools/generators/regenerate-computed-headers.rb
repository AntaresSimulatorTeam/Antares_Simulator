
require 'pathname'


class File
	def self.find(dir, filename="*.*", subdirs=true)
		Dir[ subdirs ? File.join(dir.split(/\\/), "**", filename) : File.join(dir.split(/\\/), filename) ]
	end
end


puts "Regenerating computed headers"
puts "Warning: A stupid detection of PHP scripts is preset. Please remove it as soon as possible."
count = 0
rootPath = Pathname.new(File.dirname(__FILE__) + "/../../").realpath.to_s
puts "Started from #{rootPath}"
File.find(rootPath, "*.generator.hpp").each do |f|

	# Try to detect PHP scripts. Really ugly but it will be removed anyway
	data = IO.read(f)
	isPHP = data.index('<?php') != nil

	headerFile = f[0..f.length() - 15]
	s = f.gsub(rootPath, "<path>")
	if (isPHP)
		puts " . #{s} (php)"
	else
		puts " . #{s}"
	end
	if (!isPHP)
		output = `erb "#{f}"`
		File.open(headerFile, 'w') {|f| f.write(output) }
		count += 1
	end
end
puts "Processed #{count} files."
puts "Done."
