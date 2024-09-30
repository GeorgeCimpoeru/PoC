import LoginButton from '@/src/components/loginComponents/LoginButton';
import InputsLoginPage from '@/src/components/loginComponents//InputsLoginPage';
import GoToRegister from '@/src/components/loginComponents//GoToRegister';
import LoginTitle from '@/src/components/loginComponents//LoginTitle';

const LoginPage = () => {
    return (
        <div className="bg-gray-200 w-full min-h-screen flex justify-center items-center">
            <div className="card card-compact w-96 bg-base-100 shadow-xl">
                <div className="w-100 p-2 bg-white rounded-x1 fit-content">
                    <LoginTitle />
                    <InputsLoginPage />
                    <LoginButton />
                    <GoToRegister />
                </div>
            </div>
        </div>
    )
}

export default LoginPage
