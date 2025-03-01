import os
import random

def generate_lund_files(output_folder, num_files, num_events_per_file):
    """
    Generates multiple LUND files, keeping only the trigger electron and proton in each event.
    Adds a non-zero vertex z-coordinate.
    
    :param output_folder: Directory to save the files.
    :param num_files: Number of files to generate.
    :param num_events_per_file: Number of events in each file.
    """
    os.makedirs(output_folder, exist_ok=True)  # Create the folder if it doesn't exist
    
    for file_index in range(1, num_files + 1):
        filename = f"proton_electron_lund_{file_index}.lund"
        file_path = os.path.join(output_folder, filename)
        
        with open(file_path, "w") as f:
            for event_id in range(1, num_events_per_file + 1):
                num_particles = 2  # Only electron and proton are kept
                mass_number_of_target = 1 # Not sure about this one
                atomic_number_of_target = 1 # Not sure about this one
                target_polarization = 0 # Unpolarized target
                beam_polarization = 0  # Unpolarized beam
                beam_type = 11 # Electron beam
                beam_energy = 10.6  # Beam energy in GeV
                interacted_nucleon = 2212  # Proton
                process_id = 0  # Not sure about this one
                event_weight = random.uniform(0.2, 2)  # Not sure about this one
                f.write(f"{num_particles} {mass_number_of_target} {atomic_number_of_target} {target_polarization} {beam_polarization} {beam_type} {beam_energy} {interacted_nucleon} {process_id} {event_weight}\n")
                
                # === Generate random parameters for the trigger electron ===
                index_el = 1
                lifetime_el = -1
                type_el = 1 # propagated in geant4
                particle_id_el = 11
                index_parent_el = 0
                index_daughter_el = 0
                electron_px = random.uniform(-1, 1)
                electron_py = random.uniform(-1, 1)
                electron_pz = random.uniform(8, 10.6)  # Simulating 10.6 GeV energy
                electron_mass = 0.000511
                electron_energy = (electron_px**2 + electron_py**2 + electron_pz**2 + electron_mass**2)**0.5
                electron_vx = 0  # Vertex x-coordinate
                electron_vy = 0  # Vertex y-coordinate
                electron_vz = random.gauss(-3, 2.5)  # Random vertex z-coordinate with mean -3 cm and sigma 2.5 cm  

                f.write(f"{index_el} {lifetime_el} {type_el} {particle_id_el} {index_parent_el} {index_daughter_el} {electron_px:.6f} {electron_py:.6f} {electron_pz:.6f} {electron_energy:.6f} {electron_mass} {electron_vx} {electron_vy} {electron_vz:.6f}\n")

                # === Generate random parameters for the proton ===
                index_prot = 2
                lifetime_prot = 1 
                type_prot = 1 # propagated in geant4
                particle_id_prot = 2212
                index_parent_prot = 0
                index_daughter_prot = 0
                proton_px = random.uniform(-0.5, 0.5)
                proton_py = random.uniform(-0.5, 0.5)
                proton_pz = random.uniform(0, 1.5) #??? not sure about what proton pz should be
                proton_mass = 0.938
                proton_energy = (proton_px**2 + proton_py**2 + proton_pz**2 + proton_mass**2)**0.5
                proton_vx = electron_vx  # Both originate from the same vertex
                proton_vy = electron_vy  # Both originate from the same vertex
                proton_vz = electron_vz  # Both originate from the same vertex
                
                f.write(f"{index_prot} {lifetime_prot} {type_prot} {particle_id_prot} {index_parent_prot} {index_daughter_prot} {proton_px:.6f} {proton_py:.6f} {proton_pz:.6f} {proton_energy:.6f} {proton_mass} {proton_vx} {proton_vy} {proton_vz:.6f}\n")

        print(f"LUND file saved: {file_path}")

# === Example usage ===
output_folder = "../lund_files/proton_electron_lund/"  # Directory to save files
num_files = 1  # Number of files to generate
num_events_per_file = 10  # Number of events in each file

generate_lund_files(output_folder, num_files, num_events_per_file)
