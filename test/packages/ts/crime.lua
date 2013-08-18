status_crime = {}
status_crime["crime"] = ""
status_crime["crime_from"] = ""

interface_crime = {}
interface_crime["module"] = "Verbrechen"
interface_crime["active"] = { input_type = "toggle", display_name = "Verbrechen begehen" }
interface_crime["crime"] = { input_type = "dropdown", display_name = "Verbrechen" }

function run_crime()
  return on_finish(10)
end
