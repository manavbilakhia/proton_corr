import ROOT
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
                index_electron = 1
                lifetime_electron = -1
                type_electron = 1 # propagated in geant4
                particle_id_electron = 11
                index_parent_electron = 0
                index_daughter_elctron = 0
                p_electron = random.uniform(2,8)  # electron momentum in GeV
                theta_electron = random.uniform(5, 25)  # electron theta angle in degrees
                phi_electron = random.uniform(-180, 180)
                px_electron = p_electron * ROOT.TMath.Sin(theta_electron * ROOT.TMath.DegToRad()) * ROOT.TMath.Cos(phi_electron * ROOT.TMath.DegToRad())
                py_electron = p_electron * ROOT.TMath.Sin(theta_electron * ROOT.TMath.DegToRad()) * ROOT.TMath.Sin(phi_electron * ROOT.TMath.DegToRad())
                pz_electron = p_electron * ROOT.TMath.Cos(theta_electron * ROOT.TMath.DegToRad())                                                         
                m_electron = 0.000511
                e_electron = (px_electron**2 + py_electron**2 + pz_electron**2 + m_electron**2)**0.5
                vx_electron = 0  # Vertex x-coordinate
                vy_electron = 0  # Vertex y-coordinate
                vz_electron = random.gauss(-3, 2.5)  # Random vertex z-coordinate with mean -3 cm and sigma 2.5 cm  

                f.write(f"{index_electron} {lifetime_electron} {type_electron} {particle_id_electron} {index_parent_electron} {index_daughter_elctron} {px_electron:.6f} {py_electron:.6f} {pz_electron:.6f} {e_electron:.6f} {m_electron} {vx_electron} {vy_electron} {vz_electron:.6f}\n")
                # === Generate random parameters for the proton ===
                index_proton = 2
                lifetime_proton = 1 
                type_proton = 1 # propagated in geant4
                particle_id_proton = 2212
                index_parent_proton = 0
                index_daughter_proton = 0
                p_proton = random.uniform(0.3,5)  # Proton momentum in GeV
                theta_proton = random.uniform(3, 80)  # Proton theta angle
                phi_proton = random.uniform(-180, 180)
                px_proton = p_proton * ROOT.TMath.Sin(theta_proton * ROOT.TMath.DegToRad()) * ROOT.TMath.Cos(phi_proton * ROOT.TMath.DegToRad())
                py_proton = p_proton * ROOT.TMath.Sin(theta_proton * ROOT.TMath.DegToRad()) * ROOT.TMath.Sin(phi_proton * ROOT.TMath.DegToRad())
                pz_proton = p_proton * ROOT.TMath.Cos(theta_proton * ROOT.TMath.DegToRad())                                                            
                m_protons = 0.938
                e_proton = (px_proton**2 + py_proton**2 + pz_proton**2 + m_protons**2)**0.5
                vx_proton = vx_electron  # Both originate from the same vertex
                vy_proton = vy_electron  # Both originate from the same vertex
                vz_proton = vz_electron  # Both originate from the same vertex
                
                f.write(f"{index_proton} {lifetime_proton} {type_proton} {particle_id_proton} {index_parent_proton} {index_daughter_proton} {px_proton:.6f} {py_proton:.6f} {pz_proton:.6f} {e_proton:.6f} {m_protons} {vx_proton} {vy_proton} {vz_proton:.6f}\n")
        #print(f"LUND file saved: {file_path}")
    print(f"Generated {num_files} LUND files with {num_events_per_file} events each.")

# === Example usage ===
output_folder = "../lund_files/proton_electron_lund/"  # Directory to save files
num_files = 10000  # Number of files to generate
num_events_per_file = 10000  # Number of events in each file

generate_lund_files(output_folder, num_files, num_events_per_file)
