#pragma once

#define NEP6_WALLET

#include <neo3-cpp-core/Cryptography/Cryptography.hpp>

#include <neo3-cpp-core/IO/Json/JObject.hpp> // JObject

/*
using Neo.IO.Json;
using Neo.SmartContract;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Threading.Tasks;
using UserWallet = Neo.Wallets.SQLite.UserWallet;
*/

#include "ScryptParameters.hpp"

#include "../KeyPair.hpp"

#include "NEP6Contract.hpp"

#include "NEP6Account.hpp"

#include "../Wallet.hpp"

namespace Neo {
//
namespace Wallets {
//
namespace NEP6 {
//
class NEP6Wallet : public Wallet
{

private:
   string password;

private:
   string name;

private:
   neopt::Version version;

private:
   const Dictionary<UInt160, NEP6Account*> accounts; // unique pointers
  
private:
   //const neopt::uptr<neopt::JObject> extra;
   const nlohmann::json extra;

public:
   const ScryptParameters Scrypt;

public:
   string Name() override
   {
      return name;
   }

/*
   NEP6Wallet()
     : Scrypt{ *ScryptParameters::Default }
   {
      // TODO: delete this constructor
   }
   */

   //public override Version Version => version;
   /*
        public NEP6Wallet(string path, string name = null) : base(path)
        {
            if (File.Exists(path))
            {
                JObject wallet = JObject.Parse(File.ReadAllBytes(path));
                LoadFromJson(wallet, out Scrypt, out accounts, out extra);
            }
            else
            {
                this.name = name;
                this.version = Version.Parse("3.0");
                this.Scrypt = ScryptParameters.Default;
                this.accounts = new Dictionary<UInt160, NEP6Account>();
                this.extra = JObject.Null;
            }
        }
   */

   NEP6Wallet(const nlohmann::json& wallet)
   {
      //LoadFromJson(wallet, out Scrypt, out accounts, out extra);
      //auto [_Scrypt, _accounts, _extra] = LoadFromJson(wallet);
      LoadFromJson(wallet);
      //this->Scrypt = _Scrypt;
   }

private:
   //void LoadFromJson(JObject wallet, out ScryptParameters scrypt, out Dictionary<UInt160, NEP6Account> accounts, out JObject extra)
   void LoadFromJson(const nlohmann::json& wallet)
   {
      this->version = neopt::Version::Parse(wallet["version"].get<std::string>()); //.dump()); //AsString());
      if (this->version.Major < 3) {
         NEOPT_EXCEPTION("NEP6Wallet Format Exception");
         //throw new FormatException();
      }

      //this->name = wallet["name"]?.AsString();
      this->name = wallet.contains("name")? wallet["name"].get<std::string>() : "";  //.AsString() : "";
      ScryptParameters scrypt { ScryptParameters::FromJson(wallet["scrypt"]) };
      //neopt::uptr<neopt::JObject> accounts{ new neopt::JArray{} };
      
      //= ((neopt::JArray)wallet["accounts"]).Select(
      //   p => NEP6Account.FromJson(p, this)
      //   ).ToDictionary(p = > p.ScriptHash);
      //accounts = ((neopt::JArray)wallet["accounts"]).Select(p = > NEP6Account.FromJson(p, this)).ToDictionary(p = > p.ScriptHash);

      Dictionary<UInt160, NEP6Account*>& _accounts = const_cast<Dictionary<UInt160, NEP6Account*>&>(this->accounts);
      //for(auto& [p, value] : wallet["accounts"].items())
      for(auto& p : wallet["accounts"])
      {
         neopt::uptr<NEP6Account> acc = NEP6Account::FromJson(p, *this);
         NEP6Account* pacc = acc.release();
         _accounts[acc->ScriptHash] = pacc;
      }

      

      ScryptParameters& _Scrypt = const_cast<ScryptParameters&>(this->Scrypt);
      
      // return tuple...
      //return std::make_tuple(scrypt, );
      _Scrypt = scrypt;

      nlohmann::json extra = wallet["extra"];

      nlohmann::json& _extra = const_cast<nlohmann::json&>(this->extra);
      
      //this->accounts = std::move(accounts);
      _extra = extra;
   }
   /*
        private void AddAccount(NEP6Account account, bool is_import)
        {
            lock (accounts)
            {
                if (accounts.TryGetValue(account.ScriptHash, out NEP6Account account_old))
                {
                    account.Label = account_old.Label;
                    account.IsDefault = account_old.IsDefault;
                    account.Lock = account_old.Lock;
                    if (account.Contract == null)
                    {
                        account.Contract = account_old.Contract;
                    }
                    else
                    {
                        NEP6Contract contract_old = (NEP6Contract)account_old.Contract;
                        if (contract_old != null)
                        {
                            NEP6Contract contract = (NEP6Contract)account.Contract;
                            contract.ParameterNames = contract_old.ParameterNames;
                            contract.Deployed = contract_old.Deployed;
                        }
                    }
                    account.Extra = account_old.Extra;
                }
                accounts[account.ScriptHash] = account;
            }
        }

        public override bool Contains(UInt160 scriptHash)
        {
            lock (accounts)
            {
                return accounts.ContainsKey(scriptHash);
            }
        }
*/

public:
   virtual neopt::uptr<WalletAccount> CreateAccount(const vbyte& privateKey) override
   //WalletAccount CreateAccount(byte[] privateKey)
   {
      std::cout << "Running CreateAccount with priv: " << privateKey.size() << std::endl;
      //KeyPair key = new KeyPair(privateKey);
      KeyPair key(privateKey);
      neopt::uptr<Contract> contract{
         new NEP6Contract{
           Contract::CreateSignatureRedeemScript(key.PublicKey),
           vector<ContractParameterType>{ ContractParameterType::Signature },
           vector<string>{ "signature" },
           false }
      };

      std::cout << "making account" << std::endl;
      neopt::uptr<WalletAccount> account{
         new NEP6Account(*this, contract->ScriptHash(), key, password, contract.release())
      };

      //AddAccount(account, false);

      return account;
   }

   /*
        public override WalletAccount CreateAccount(Contract contract, KeyPair key = null)
        {
            NEP6Contract nep6contract = contract as NEP6Contract;
            if (nep6contract == null)
            {
                nep6contract = new NEP6Contract
                {
                    Script = contract.Script,
                    ParameterList = contract.ParameterList,
                    ParameterNames = contract.ParameterList.Select((p, i) => $"parameter{i}").ToArray(),
                    Deployed = false
                };
            }
            NEP6Account account;
            if (key == null)
                account = new NEP6Account(this, nep6contract.ScriptHash);
            else
                account = new NEP6Account(this, nep6contract.ScriptHash, key, password);
            account.Contract = nep6contract;
            AddAccount(account, false);
            return account;
        }

        public override WalletAccount CreateAccount(UInt160 scriptHash)
        {
            NEP6Account account = new NEP6Account(this, scriptHash);
            AddAccount(account, true);
            return account;
        }
*/
public:
   neopt::uptr<KeyPair> DecryptKey(string nep2key) const
   {
      return neopt::uptr<KeyPair>{
         // TODO: verify if std::optional is broken
         new KeyPair{
           *GetPrivateKeyFromNEP2(nep2key, password, Scrypt.N, Scrypt.R, Scrypt.P) }
      };
   }

   /*
        public override bool DeleteAccount(UInt160 scriptHash)
        {
            lock (accounts)
            {
                return accounts.Remove(scriptHash);
            }
        }

        public override WalletAccount GetAccount(UInt160 scriptHash)
        {
            lock (accounts)
            {
                accounts.TryGetValue(scriptHash, out NEP6Account account);
                return account;
            }
        }

        public override IEnumerable<WalletAccount> GetAccounts()
        {
            lock (accounts)
            {
                foreach (NEP6Account account in accounts.Values)
                    yield return account;
            }
        }

        public override WalletAccount Import(X509Certificate2 cert)
        {
            KeyPair key;
            using (ECDsa ecdsa = cert.GetECDsaPrivateKey())
            {
                key = new KeyPair(ecdsa.ExportParameters(true).D);
            }
            NEP6Contract contract = new NEP6Contract
            {
                Script = Contract.CreateSignatureRedeemScript(key.PublicKey),
                ParameterList = new[] { ContractParameterType.Signature },
                ParameterNames = new[] { "signature" },
                Deployed = false
            };
            NEP6Account account = new NEP6Account(this, contract.ScriptHash, key, password)
            {
                Contract = contract
            };
            AddAccount(account, true);
            return account;
        }

        public override WalletAccount Import(string wif)
        {
            KeyPair key = new KeyPair(GetPrivateKeyFromWIF(wif));
            NEP6Contract contract = new NEP6Contract
            {
                Script = Contract.CreateSignatureRedeemScript(key.PublicKey),
                ParameterList = new[] { ContractParameterType.Signature },
                ParameterNames = new[] { "signature" },
                Deployed = false
            };
            NEP6Account account = new NEP6Account(this, contract.ScriptHash, key, password)
            {
                Contract = contract
            };
            AddAccount(account, true);
            return account;
        }

        public override WalletAccount Import(string nep2, string passphrase, int N = 16384, int r = 8, int p = 8)
        {
            KeyPair key = new KeyPair(GetPrivateKeyFromNEP2(nep2, passphrase, N, r, p));
            NEP6Contract contract = new NEP6Contract
            {
                Script = Contract.CreateSignatureRedeemScript(key.PublicKey),
                ParameterList = new[] { ContractParameterType.Signature },
                ParameterNames = new[] { "signature" },
                Deployed = false
            };
            NEP6Account account;
            if (Scrypt.N == 16384 && Scrypt.R == 8 && Scrypt.P == 8)
                account = new NEP6Account(this, contract.ScriptHash, nep2);
            else
                account = new NEP6Account(this, contract.ScriptHash, key, passphrase);
            account.Contract = contract;
            AddAccount(account, true);
            return account;
        }

        internal void Lock()
        {
            password = null;
        }

        public static NEP6Wallet Migrate(string path, string db3path, string password)
        {
            UserWallet wallet_old = UserWallet.Open(db3path, password);
            NEP6Wallet wallet_new = new NEP6Wallet(path, wallet_old.Name);
            using (wallet_new.Unlock(password))
            {
                foreach (WalletAccount account in wallet_old.GetAccounts())
                {
                    wallet_new.CreateAccount(account.Contract, account.GetKey());
                }
            }
            return wallet_new;
        }

        public void Save()
        {
            JObject wallet = new JObject();
            wallet["name"] = name;
            wallet["version"] = version.ToString();
            wallet["scrypt"] = Scrypt.ToJson();
            wallet["accounts"] = new JArray(accounts.Values.Select(p => p.ToJson()));
            wallet["extra"] = extra;
            File.WriteAllText(Path, wallet.ToString());
        }

        public IDisposable Unlock(string password)
        {
            if (!VerifyPassword(password))
                throw new CryptographicException();
            this.password = password;
            return new WalletLocker(this);
        }

        public override bool VerifyPassword(string password)
        {
            lock (accounts)
            {
                NEP6Account account = accounts.Values.FirstOrDefault(p => !p.Decrypted);
                if (account == null)
                {
                    account = accounts.Values.FirstOrDefault(p => p.HasKey);
                }
                if (account == null) return true;
                if (account.Decrypted)
                {
                    return account.VerifyPassword(password);
                }
                else
                {
                    try
                    {
                        account.GetKey(password);
                        return true;
                    }
                    catch (FormatException)
                    {
                        return false;
                    }
                }
            }
        }

        public override bool ChangePassword(string oldPassword, string newPassword)
        {
            bool succeed = true;
            lock (accounts)
            {
                Parallel.ForEach(accounts.Values, (account, state) =>
                {
                    if (!account.ChangePasswordPrepare(oldPassword, newPassword))
                    {
                        state.Stop();
                        succeed = false;
                    }
                });
            }
            if (succeed)
            {
                foreach (NEP6Account account in accounts.Values)
                    account.ChangePasswordCommit();
                if (password != null)
                    password = newPassword;
            }
            else
            {
                foreach (NEP6Account account in accounts.Values)
                    account.ChangePasswordRoolback();
            }
            return succeed;
        }
        */

}; // Class NEP6Wallet
//
} // namespace NEP6
//
} // namespace Wallets
//
} // namespace Neo
  //
