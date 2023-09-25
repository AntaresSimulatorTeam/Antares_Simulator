
require "ostruct"
require 'timeout'
require 'myopen4'

# -2 : invalid
# -1 : timeout
#  0 : failed
#  1 : ok



def loadConfigFile(path)
	eval <<-END_CONFIG
	options = OpenStruct.new
	#{File.read(path)}
	options
	END_CONFIG
end



def compile(options)
	options.compilation = OpenStruct.new
	options.compilation.message = ''
	options.compilation.result = 0
	options.compilation.stdout = ''
	options.compilation.stderr = ''
	t = 10 * 60
	status = 0
	# Timeout for compiling
	timeout(t) {
		status = POpen4.popen4(options.make) do |stdout, stderr, stdin, pid|
			while (s = stderr.gets) != nil
				options.compilation.stderr += s
			end
			while (s = stdout.gets) != nil
				options.compilation.stdout += s
			end
		end
	}
	if status == 0
		options.compilation.result = 1
	else
		options.compilation.result = 0
		options.compilation.message += "The compilation has failed\n"
	end
	rescue Timeout::Error
	options.compilation.message += "Compilation failed: Timeout (after " + t + " seconds)\n"
	options.compilation.result = -1
end



def execute(options)
	options.executable = OpenStruct.new
	options.executable.message = ''
	options.executable.result = 1
	options.executable.stdout = ''
	options.executable.stdoutExpected = nil
	options.executable.stderr = ''
	options.executable.stderrExpected = nil
	options.executable.command = options.program
	options.executable.timeout = options.timeout
	options.executable.duration = 0
	if options.executable.timeout < 3
		options.executable.timeout = 3
	else
		if options.executable.timeout > 60
			options.executable.timeout = 60
		end
	end
	if options.compilation.result != 1
		options.executable.message += "Not executed\n"
		options.executable.result = -2
		return
	end
	status = 0
	timeout(options.executable.timeout) {
		start = Time.now.to_f
		status = POpen4.popen4(options.executable.command) do |stdout, stderr, stdin, pid|
			while (s = stderr.gets) != nil
				options.executable.stderr += s
			end
			while (s = stdout.gets) != nil
				options.executable.stdout += s
			end
		end
		e = Time.now.to_f
		options.executable.duration = (e - start) * 1000 # ms
	}
	# Output comparison
	if options.compareStdoutWith != nil and not options.compareStdoutWith.empty?
		filename = options.path + '/' + options.compareStdoutWith
		if File.exist?(filename)
			f = File.new(filename)
			text = f.read
			if text != options.executable.stdout
				options.executable.result = 0
				options.executable.message += "The stdout is not as expected.\n"
				options.executable.stdoutExpected = text
			end
		else
			options.executable.result = 0
			options.executable.message += "The file of reference for stdout has not been found.\n"
		end
	end
	if options.compareStderrWith != nil and not options.compareStderrWith.empty?
		filename = options.path + '/' + options.compareStderrWith
		if File.exist?(filename)
			f = File.new(options.path + '/' + options.compareStderrWith)
			text = f.read
			if text != options.executable.stderr
				options.executable.result = 0
				options.executable.message += "The stderr is not as expected.\n"
				options.executable.stderrExpected = text
			end
		else
			options.executable.result = 0
			options.executable.message += "The file of reference for stderr has not been found.\n"
		end
	end
	if options.exitStatus != status
		options.executable.result = 0
		options.executable.message += "The exit status is incorrect.\n"
	end

	# Rescue
	rescue Timeout::Error
	options.executable.message += "Execution failed: Timeout (after " + options.executable.timeout.to_s
	options.executable.message += " seconds).\n"
	options.executable.result = -1
end


def resultToStr(value)
	case value
		when -2: "invalid"
		when -1: "timeout"
		when  1: "ok"
		else "failed"
	end
end


def writeXMLTest(tag, t)
	if t != nil and not t.empty?
		return "\t\t<#{tag}><![CDATA[#{t}]]></#{tag}>\n"
	end
	""
end


def generateReport(options)
	r = ''
	r += "<testcase id=\"" + options.identifier + "\" result=\"" + resultToStr(options.result) + "\">\n"
	r += "\t<compilation result=\"%s\">\n" % resultToStr(options.compilation.result)
	r += writeXMLTest("msg", options.compilation.message)
	r += writeXMLTest("stdout", options.compilation.stdout)
	r += writeXMLTest("stderr", options.compilation.stderr)
	r += "\t</compilation>\n"
	r += "\t<exec result=\"%s\">\n" % resultToStr(options.executable.result)
	r += writeXMLTest("cmd", options.executable.command)
	r += writeXMLTest("msg", options.executable.message)
	r += writeXMLTest("stdout", options.executable.stdout)
	r += writeXMLTest("stderr", options.executable.stderr)
	r += writeXMLTest("stdoutexpected", options.executable.stdoutExpected)
	r += writeXMLTest("stderrexpected", options.executable.stderrExpected)
	if options.executable.duration > 0
		r += "\t\t<duration>%.0f</duration>\n" % options.executable.duration
	end
	r += "\t</exec>\n"

	r += "</testcase>"
	File.open(options.report, 'w') {|f| f.write(r) }
end



def executeTestcase(filename)
	o = loadConfigFile(filename)
	if nil != o.identifier and "" != o.identifier
		#puts "Running testcase #{o.identifier}"
		# Try to compile
		compile(o)
		# Execute the program
		execute(o)

		# Global result for the testcase
		if o.executable.result == -1
			o.result = -1
		else
			o.result = (o.executable.result == 1 and o.compilation.result == 1) ? 1 : 0
		end

		# Generate the report
		generateReport(o)
	end
end


begin
	ARGV.each do|a|
		executeTestcase(a)
	end
end

