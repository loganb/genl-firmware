#!/usr/bin/env ruby
require 'rubygems'
require 'serialport'
require 'optparse'

options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options]"

  opts.on("-v", "--[no-]verbose", "Run verbosely") do |v|
    options[:verbose] = v
  end
end.parse!

if(options[:verbose])
  $stderr.puts("Running in verbose mode")
end

@file = ARGV.shift
puts "Opening serial port #{@file}"

@sp = SerialPort.new @file, 9600

puts "Setting the bits"
@sp.dtr = 1
@sp.rts = 1

loop do
  bytes = @sp.readline("\0").bytes.to_a
  
  p "Read: #{bytes.join(' ')}"
end