require 'bigdecimal'

(0..64).each do |i|
  #32-steps per quarter turn
  theta = (i/32.0) * 2 * 3.14159265 / 4
  
  tmp = BigDecimal((63 * Math.cos(theta)).to_s).round.to_i
  
  STDOUT.puts(tmp)
end