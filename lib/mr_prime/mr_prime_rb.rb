# Thanks Bruce Schneier for this gem
def exponent_mod(base,exponent,modulus)
  result = 1;
  while(exponent > 0) 
    if ((exponent & 1) == 1) 
      result = (result * base) % modulus;
    end
    exponent >>= 1;
    base = (base * base) % modulus;
  end

  return result;
end


# Test r&s can reconstitute n properly
class MR_Prime
  attr_accessor :number
  attr_reader :r,:s,:trials

  # Allow for easier access to instance of class via MR_Prime[n]
  def self.[](n,opts={})
    MR_Prime.new(n,opts).prime?
  end

  def self.trials_for_precision(prec)
    k = 1
    while( 1.0 / (4.0 ** k) > prec)
      k += 1
    end
    return k
  end

  def initialize(n,opts={})
    self.number = n
    set_trials(opts)
  end

  def witnessed_by?(a)
    result = exponent_mod(a,@s,@number)
    if result == 1
      return true
    else
      0.upto(@r-1) do |j|
        result = exponent_mod(a,(2**j)*@s,@number)
        if result == -1 or result == @number - 1
          return true
        end
      end
    end
    return false
  end

  def number=(n)
    @number = n
    resolve_r_and_s
  end

  def prime?
    witnesses.all? { |a| witnessed_by? a }
  end

#private

  # Set the number of trials to the max of num required for precision, witnesses opt, or 19 (giving at max 1.0e-10 chance of failure)
  def set_trials(opts)
    trials = [19] # default precision of 0.00000000036% chance of failure
    if opts[:precision]
      if opts[:precision] == 0 # Short circuit of absolute precision is required
        @absolute = true
        @trials = -1
        return
      else
        trials.push(MR_Prime.trials_for_precision(opts[:precision]))
      end
    end
    trials.push(opts[:witnesses]) if opts[:witnesses]
    @trials = trials.max
  end

  # Find R and S such that 2^r * s + 1 == @number (as per MR def'n)
  def resolve_r_and_s
    @r,@s=0,@number-1
    while(@s.even?)
      @r += 1
      @s /= 2
    end
  end

  # Witnesses require for absoulte certainty for any number < key
  #  OR
  # the number of trials specified by :witnesses or :precision arguments
  # Source Wikipedia and "The Primes Page"
  def witnesses
    witnesses = []
    if @number < 1_373_653
      witnesses = [2,3]
    elsif @number < 9_080_191
      witnesses = [31,73]
    elsif @number < 4_759_123_141
      witnesses = [2,7,61]
    elsif @number < 2_152_302_898_747
      witnesses = [2,3,5,7,11]
    elsif @number < 3_474_749_660_383
      witnesses = [2, 3, 5, 7, 11, 13]
    elsif @number < 341_550_071_728_321
      witnesses = [2, 3, 5, 7, 11, 13, 17]
    elsif @precision
      witnesses = (2...@number)
    else
      witnesses = Array.new(@trials).map { rand(@number-1)+1 }
    end
    return witnesses
  end
end

if __FILE__ == $0 and !ARGV.empty?
  puts MR_Prime[ARGV.shift.to_i]
end
