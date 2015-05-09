#!/usr/bin/env python

# This script is used to test the new method implementation
# It does that by comparing the computed prayer times for specific
# locations on specific dates to the "official" times on these dates.
# "official" turns out to be a tricky one for locations in places
# that are in non-Muslim countries.

import sys
import os
import subprocess
import json
import datetime
import unittest

program_under_test=os.path.join("..", "build", "prayer")
locations="locations"
references="reference_times"
# We require all the computed times to be within 3 minutes
# of the reference values
threshold = datetime.timedelta(minutes=3)

class TestPrayerNewMethod(unittest.TestCase):
    def setUp(self):
        self.test_files = []
        for f in os.listdir(references):
            fn = os.path.join(references,f)
            if os.path.isfile(fn):
                self.test_files.append(fn)

    def test_locations(self):
        for f in self.test_files:
            fp = open(f)
            json_data = json.load(fp)
            fp.close()
            country = json_data["country"]
            location = json_data["location"]
            date = json_data["date"]
            timezone = json_data["tz"] # Not used for now
            dst = json_data["dst"]     # Not used for now
            command = program_under_test + \
                    " -d " + date + \
                    " -f " + \
                    os.path.join(locations, country, \
                            country + "-" + location + ".conf") + \
                    " -j"
            p = subprocess.Popen(command,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    shell=True)
            (output, errors) = p.communicate()
            self.assertEqual(p.returncode, 0)
            self.assertEqual(errors, "")            
            ref_times = list(json_data["times"])
            computed_times = list(json.loads(output)["times"])
            self.assertEqual(len(ref_times), len(computed_times))
            for i in range(len(computed_times)):
                ref_x = ref_times[i].items()[0][1]
                comp_x = computed_times[i].items()[0][1]
                time_format = "%H:%M"
                t_r = datetime.datetime.strptime(ref_x, time_format)
                t_c = datetime.datetime.strptime(comp_x, time_format)
                if t_r > t_c:
                    tdelta = t_r - t_c
                else:
                    tdelta = t_c - t_r
                self.assertTrue(tdelta < threshold)

if __name__ == "__main__":
    unittest.main()

