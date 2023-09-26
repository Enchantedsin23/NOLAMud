#!/usr/bin/perl -w
use strict;
# use Switch;
use Net::FTP;


if (!defined($ARGV[0]))
{
	print "Please enter the four letter code for your station. i.e. ./get_weather LEBA\n";
	exit;
}

my $station_name = $ARGV[0];

# if (-e "$station_name.TXT")
# {
# 	rename("$station_name.TXT", "$station_name.OLD");
# }
# else
# {
# 	open(BLANK_FILE, ">$station_name.OLD");
# 	close(BLANK_FILE);
# }

my $ftp = Net::FTP->new("tgftp.nws.noaa.gov", DEBUG => 1, Passive => 1);

$ftp->login("anonymous", "guest") 
  or die "login error - " , $ftp->message;
$ftp->cwd("data/observations/metar/stations") 
  or die "cwd failed - ", $ftp->message;
$ftp->ascii;
$ftp->get("$station_name.TXT", "../area/weather/$station_name.TXT") 
  or die "failed to retrieve file $station_name - ", $ftp->message;
$ftp->quit;
# End ftp session #


if (!(-e "../area/weather/$station_name.TXT"))
{
	print "Error: ", $station_name, " could not be found!\n";
#	unlink("$station_name.OLD"); #Remove the generated .OLD file
	exit;
}

# Updating weather file
&interpret_weather($station_name);

sub parse_time
{
	my $time = int(substr($_[0], 2, 2));

	print OUTPUT "time: ", $time, ":", substr($_[0], 4, 2), "\n";
}	

sub parse_wind
{
	if ($_[0] =~ /^VRB/)
	{
		print OUTPUT "wind_dir: var\nwind_spd: ", substr($_[0], 3, 2), "\nclouds: ";
	}
	else
	{
		print OUTPUT "wind_dir: ", &get_direction(int(substr($_[0], 0, 3))), "\nwind_spd: ", substr($_[0], 3, 2), "\nclouds: ";
	}
}

sub parse_temp
{
        if ($_[0] =~ /^M/)
        {
       		print OUTPUT "\ntemp: -", substr($_[0], 1, 2);
        }
        else
        {
  		print OUTPUT "\ntemp: ", substr($_[0], 0, 2);
        }

        print OUTPUT "\n";
}

sub interpret_weather
{
	my $station_name = $_[0];

	open(INPUT, "../area/weather/$station_name.TXT") or die("Can't open $station_name.TXT!\n");
	open(OUTPUT, ">../area/weather/weather.txt") or die("Can't open weather.txt!\n");

	my @raw_data = <INPUT>;
	close(INPUT);

	foreach my $command_string (@raw_data)
	{
	        chop($command_string);
	        my @command_list = split(/ /, $command_string);

	        foreach my $weather_key (@command_list)
	        {
			if ($weather_key =~ m/^RMK/)
			{
				# Remarks are for suckers so we're done and need to exit
				close(OUTPUT);	
			#	unlink("../area/weather/$station_name.TXT"); #Remove the METAR file
				exit;
			}
			elsif ($weather_key =~ m/^AUTO|^COR/)
			{
				# This tells us if the report is automatically generated, or corrected manually
				# and we don't care
			}
			elsif ($weather_key =~ m/FT$|SM$|^VV/)
			{
				# Values we don't care about
				#  - Runway Visibilitiy
				#  - General Visibility
				#  - Sky Condition
			}
	                elsif ($weather_key =~ m/[^A-Y]Z$/)
                        {
                                &parse_time($weather_key);
                        }
	                elsif ($weather_key =~ m/KT$/)
                        {
                                &parse_wind($weather_key);
                        }
	                elsif ($weather_key =~ m/^SKC|^CLR|CAVOK/)
                        {
				print OUTPUT "clear";
                        }
			elsif ($weather_key =~ m/^FEW/)
			{
				print OUTPUT "few ", substr($weather_key, 3, 3), " ";
			}
	    elsif ($weather_key =~ m/^SCT/)
      {
        print OUTPUT "scattered ", substr($weather_key, 3, 3), " ";
      }
			elsif ($weather_key =~ m/^BKN/)
			{
				print OUTPUT "broken ", substr($weather_key, 3, 3), " ";
			}
			elsif ($weather_key =~ m/^OVC/)
			{
				print OUTPUT "overcast ", substr($weather_key, 3, 3), " ";
			}
	                elsif ($weather_key =~ m/\//)
                        {
                                if (length($weather_key) <= 7)
                                {
                                        parse_temp($weather_key);
                                }
                        }
			elsif ($weather_key =~ m/^([\+-]|[A-Z]){2,}[^0-9]*/)
			{
				if ($weather_key ne $station_name)
				{
					#print OUTPUT parse_weather($weather_key);
				}
			}
                        else
                        { 
                       	 	# Extra Key Found..
                        }
	        }
	}

	close(OUTPUT);
#	unlink("../area/weather/$station_name.TXT"); #Remove the METAR file
}

sub get_direction
{
	my $direction = $_[0] - 22.5;

	if ($direction <= 0)
	{
		return "N";
	}
	elsif ($direction <= 45)
	{
		return "NE";
	}
	elsif ($direction <= 90)
	{
		return "E";
	}
	elsif ($direction <= 135)
	{
		return "SE";
	}
	elsif ($direction <= 180)
	{
		return "S";
	}
	elsif ($direction <= 225)
	{
		return "SW";
	}
	elsif ($direction <= 270)
	{
		return "W";
	}
	elsif ($direction <= 315)
	{
		return "NW";
	}
	else
	{
		return "N";
	}
}


sub parse_weather
{
	my $final_weather = "weather: ";
	my $weather_code = $_[0];

	my %weather_decoder = (
	"MI" => "shallow ",
	"PR" => "partial ",
	"BC" => "patches ",
	"DR" => "low drifting ",
	"BL" => "blowing ",
	"SH" => "shower ",
	"TS" => "thunderstorm ",
	"FZ" => "freezing ",
	"DZ" => "drizzle",
	"RA" => "rain",
	"SN" => "snow",
	"SG" => "snow grains",
	"IC" => "ice crystals",
	"PL" => "ice pellets",
	"GR" => "hail",
	"GS" => "small hail",
	"UP" => "unknown precipitation",
	"BR" => "mist",
	"FG" => "fog",
	"FU" => "smoke",
	"VA" => "volcanic ash",
	"DU" => "widespread dust",
	"SA" => "sand",
	"HZ" => "haze",
	"PY" => "spray",
	"PO" => "wussnado",
	"SQ" => "squalls",
	"FC" => "tornado",
	"SS" => "sandstorm",
	"DS" => "duststorm"
	);
	
	if ($weather_code =~ /\+/)
	{
		$final_weather .= "heavy ";
		# Trim the +
		$weather_code = substr($weather_code, 1);
	}
	elsif($weather_code =~ /\-/)
	{
		$final_weather .= "light ";
		# Trim the -
		$weather_code = substr($weather_code, 1);
	}
	elsif($weather_code =~ /^VC/)
	{
		$final_weather .= "nearby ";
		# Trim the VC
		$weather_code = substr($weather_code, 2);
	}
	else
	{
		$final_weather .= "moderate ";
	}
	
	while (length($weather_code) > 0)
	{
		#take off the first two characters and put them in temp
		my $temp = substr($weather_code, 0, 2);
		$weather_code = substr($weather_code, 2);
		$final_weather .= $weather_decoder{$temp};
	}
	$final_weather .= "\n";

	return $final_weather;
}
